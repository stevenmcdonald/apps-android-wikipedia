package org.wikipedia.main

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.view.ActionMode
import androidx.appcompat.widget.Toolbar
import androidx.fragment.app.Fragment
import org.wikipedia.Constants
import org.wikipedia.R
import org.wikipedia.activity.SingleFragmentActivity
import org.wikipedia.databinding.ActivityMainBinding
import org.wikipedia.navtab.NavTab
import org.wikipedia.onboarding.InitialOnboardingActivity
import org.wikipedia.settings.Prefs
import org.wikipedia.util.DimenUtil
import org.wikipedia.util.FeedbackUtil
import org.wikipedia.util.ResourceUtil
import android.content.BroadcastReceiver
import android.content.IntentFilter
import android.util.Log
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import androidx.core.content.ContextCompat
import org.greatfire.envoy.*

import IPtProxy.IPtProxy
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import org.json.JSONArray
import org.json.JSONObject
import org.wikipedia.BuildConfig
import java.io.BufferedReader
import java.io.FileNotFoundException
import java.lang.Exception
import java.net.ConnectException
import java.net.HttpURLConnection
import java.net.SocketTimeoutException
import java.net.URL

class MainActivity : SingleFragmentActivity<MainFragment>(), MainFragment.Callback {

    private val TAG = "MainActivity"

    private lateinit var binding: ActivityMainBinding

    private var controlNavTabInFragment = false

    // initialize one or more string values containing the urls of available http/https proxies (include trailing slash)
    // -> now parsing urls from dnstt request
    // urls for additional proxy services, change if there are port conflicts (do not include trailing slash)
    private val ssUrlLocal = "socks5://127.0.0.1:1080"
    private var ssUrlRemote = ""
    // add all string values to this list value
    private val possibleUrls = mutableListOf<String>()

    // TODO: revisit and refactor
    private var waitingForDnstt = false
    private var waitingForShadowsocks = false
    private var waitingForUrl = false

    // this receiver should be triggered by a success or failure broadcast from either the
    // NetworkIntentService (indicating whether submitted urls were valid or invalid) or the
    // ShadowsocksService (indicating whether the service was successfully started or not
    private val mBroadcastReceiver: BroadcastReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            if (intent != null && context != null) {
                if (intent.action == BROADCAST_URL_VALIDATION_SUCCEEDED) {
                    val validUrls = intent.getStringArrayListExtra(EXTENDED_DATA_VALID_URLS)
                    Log.d(TAG, "received " + validUrls?.size + " valid urls")
                    if (waitingForUrl) {
                        if (validUrls != null && !validUrls.isEmpty()) {
                            waitingForUrl = false
                            val envoyUrl = validUrls[0]
                            Log.d(TAG, "found a valid url, start engine")
                            // select the fastest one (urls are ordered by latency), reInitializeIfNeeded set to false
                            CronetNetworking.initializeCronetEngine(context, envoyUrl)
                        } else {
                            Log.e(TAG, "received empty list of valid urls")
                        }
                    } else {
                        Log.d(TAG, "already found a valid url")
                    }
                } else if (intent.action == BROADCAST_URL_VALIDATION_FAILED) {
                    val invalidUrls = intent.getStringArrayListExtra(EXTENDED_DATA_INVALID_URLS)
                    Log.e(TAG, "received " + invalidUrls?.size + " invalid urls")
                    // TODO: log or show error if all possible urls were invalid?
                } else if (intent.action == ShadowsocksService.SHADOWSOCKS_SERVICE_BROADCAST) {
                    waitingForShadowsocks = false
                    var shadowsocksResult = intent.getIntExtra(ShadowsocksService.SHADOWSOCKS_SERVICE_RESULT, 0)
                    if (shadowsocksResult > 0) {
                        Log.d(TAG, "shadowsocks service started ok")
                    } else {
                        Log.e(TAG, "shadowsocks service failed to start")
                    }
                    // service was started if possible, submit list of urls to envoy for evaluation
                    waitingForUrl = true
                    NetworkIntentService.submit(this@MainActivity, possibleUrls)
                } else {
                    Log.e(TAG, "received unexpected intent: " + intent.action)
                }
            } else {
                Log.e(TAG, "receiver triggered but context or intent was null")
            }
        }
    }

    override fun inflateAndSetContentView() {
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    private fun envoySetup() {

        // immediately submit default url in case dnstt fails
        if (BuildConfig.DEF_PROXY.isNullOrEmpty()) {
            Log.w(TAG, "no default proxy url was provided")
        } else {
            Log.d(TAG, "submit default proxy url to envoy")
            waitingForUrl = true
            NetworkIntentService.submit(this, mutableListOf<String>(BuildConfig.DEF_PROXY))
        }

        // start asynchronous dnstt task to fetch proxy urls
        lifecycleScope.launch(Dispatchers.IO) {
            getDnsttMetadata()
        }
    }

    fun getDnsttMetadata() {

        // check for dnstt project properties
        if (BuildConfig.DNSTT_SERVER.isNullOrEmpty()
            || BuildConfig.DNSTT_KEY.isNullOrEmpty()
            || BuildConfig.DNSTT_PATH.isNullOrEmpty()
            || (BuildConfig.DOH_URL.isNullOrEmpty() && BuildConfig.DOT_ADDR.isNullOrEmpty())) {
            Log.e(TAG, "dnstt parameters are not defined, cannot fetch metadata with dnstt")
        } else {

            // set time limit for dnstt (dnstt allows a long timeout and retries, may never return)
            lifecycleScope.launch(Dispatchers.IO) {
                Log.d(TAG, "start timer")
                waitingForDnstt = true
                delay(10000L)  // wait 10 seconds
                if (waitingForDnstt) {
                    Log.d(TAG, "stop timer, stop dnstt")
                    waitingForDnstt = false
                    IPtProxy.stopDNSttProxy()
                } else {
                    Log.d(TAG, "dnstt already complete")
                }
            }

            try {
                // provide either DOH or DOT address, and provide an empty string for the other
                Log.d(TAG, "start dnstt proxy: " + BuildConfig.DNSTT_SERVER + " / " + BuildConfig.DOH_URL + " / " + BuildConfig.DOT_ADDR + " / " + BuildConfig.DNSTT_KEY)
                val dnsttPort = IPtProxy.startDNSttProxy(
                    BuildConfig.DNSTT_SERVER,
                    BuildConfig.DOH_URL,
                    BuildConfig.DOT_ADDR,
                    BuildConfig.DNSTT_KEY
                )

                Log.d(TAG, "get list of possible urls")
                val url = URL("http://127.0.0.1:" + dnsttPort + BuildConfig.DNSTT_PATH)
                Log.d(TAG, "open connection: " + url)
                val connection = url.openConnection() as HttpURLConnection
                try {
                    Log.d(TAG, "set timeout")
                    connection.connectTimeout = 5000
                    Log.d(TAG, "connect")
                    connection.connect()
                } catch (e: SocketTimeoutException) {
                    Log.e(TAG, "connection timeout when connecting: " + e.localizedMessage)
                } catch (e: ConnectException) {
                    Log.e(TAG, "connection error: " + e.localizedMessage)
                } catch (e: Exception) {
                    Log.e(TAG, "unexpected error when connecting: " + e.localizedMessage)
                }

                try {
                    Log.d(TAG, "open input stream")
                    val input = connection.inputStream
                    if (input != null) {
                        Log.d(TAG, "parse json and extract possible urls")
                        val json = input.bufferedReader().use(BufferedReader::readText)
                        val envoyObject = JSONObject(json)
                        val envoyUrlArray = envoyObject.getJSONArray("envoyUrls")
                        handleDnsttResults(envoyUrlArray)
                    } else {
                        Log.e(TAG, "response contained no json to parse")
                    }
                } catch (e: SocketTimeoutException) {
                    Log.e(TAG, "connection timeout when getting input: " + e.localizedMessage)
                } catch (e: FileNotFoundException) {
                    Log.e(TAG, "config file error: " + e.localizedMessage)
                } catch (e: Exception) {
                    Log.e(TAG, "unexpected error when reading file: " + e.localizedMessage)
                }
            } catch (e: Error) {
                Log.e(TAG, "dnstt error: " + e.localizedMessage)
            } catch (e: Exception) {
                Log.e(TAG, "unexpected error when starting dnstt: " + e.localizedMessage)
            }

            Log.d(TAG, "stop dnstt proxy")
            waitingForDnstt = false
            IPtProxy.stopDNSttProxy()
        }
    }

    fun handleDnsttResults(envoyUrlArray: JSONArray?) {

        // parse dnstt urls
        for (i in 0 until envoyUrlArray!!.length()) {
            if (envoyUrlArray.getString(i).startsWith("ss://")) {
                Log.d(TAG, "found ss url")
                possibleUrls.add(ssUrlLocal)
                ssUrlRemote = envoyUrlArray.getString(i)
            } else {
                Log.d(TAG, "found url")
                possibleUrls.add(envoyUrlArray.getString(i))
            }
        }

        // check for urls that require services
        for (url in possibleUrls) {
            // Notification.Builder in ShadowsocksService.onStartCommand may require api > 7
            if (url.startsWith("socks5://")) {
                Log.d(TAG, "shadowsocks service needed, submit urls after starting")
                // start shadowsocks service
                val shadowsocksIntent = Intent(this, ShadowsocksService::class.java)
                // put shadowsocks proxy url here, should look like ss://Y2hhY2hhMjAtaWV0Zi1wb2x5MTMwNTpwYXNz@127.0.0.1:1234 (base64 encode user/password)
                shadowsocksIntent.putExtra(
                    "org.greatfire.envoy.START_SS_LOCAL",
                    ssUrlRemote
                );
                waitingForShadowsocks = true
                ContextCompat.startForegroundService(applicationContext, shadowsocksIntent)
                return;
            }
        }

        if (possibleUrls.isEmpty()) {
            Log.w(TAG, "no urls to submit, cannot setup envoy")
        } else {
            // submit list of urls to envoy for evaluation
            Log.d(TAG, "no services needed, submit urls immediately")
            waitingForUrl = true
            NetworkIntentService.submit(this, possibleUrls)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // register to receive test results
        LocalBroadcastManager.getInstance(this).registerReceiver(mBroadcastReceiver, IntentFilter().apply {
            addAction(BROADCAST_URL_VALIDATION_SUCCEEDED)
            addAction(BROADCAST_URL_VALIDATION_FAILED)
            addAction(ShadowsocksService.SHADOWSOCKS_SERVICE_BROADCAST)
        })

        // run envoy setup (fetches and validate urls)
        envoySetup()

        setImageZoomHelper()
        if (Prefs.isInitialOnboardingEnabled && savedInstanceState == null) {
            // Updating preference so the search multilingual tooltip
            // is not shown again for first time users
            Prefs.isMultilingualSearchTooltipShown = false

            // Use startActivityForResult to avoid preload the Feed contents before finishing the initial onboarding.
            // The ACTIVITY_REQUEST_INITIAL_ONBOARDING has not been used in any onActivityResult
            startActivityForResult(InitialOnboardingActivity.newIntent(this), Constants.ACTIVITY_REQUEST_INITIAL_ONBOARDING)
        }
        setNavigationBarColor(ResourceUtil.getThemedColor(this, R.attr.nav_tab_background_color))
        setSupportActionBar(binding.mainToolbar)
        supportActionBar?.title = ""
        supportActionBar?.setDisplayHomeAsUpEnabled(false)
        binding.mainToolbar.navigationIcon = null
    }

    override fun onResume() {
        super.onResume()
        invalidateOptionsMenu()
    }

    override fun createFragment(): MainFragment {
        return MainFragment.newInstance()
    }

    override fun onTabChanged(tab: NavTab) {
        binding.mainToolbar.setTitle(tab.text())
        if (tab == NavTab.EXPLORE) {
            controlNavTabInFragment = false
        } else {
            if (tab == NavTab.SEARCH && Prefs.showSearchTabTooltip) {
                FeedbackUtil.showTooltip(this, fragment.binding.mainNavTabLayout.findViewById(NavTab.SEARCH.id()), getString(R.string.search_tab_tooltip), aboveOrBelow = true, autoDismiss = false)
                Prefs.showSearchTabTooltip = false
            }
            controlNavTabInFragment = true
        }
        fragment.requestUpdateToolbarElevation()
    }

    override fun onSupportActionModeStarted(mode: ActionMode) {
        super.onSupportActionModeStarted(mode)
        if (!controlNavTabInFragment) {
            fragment.setBottomNavVisible(false)
        }
    }

    override fun onSupportActionModeFinished(mode: ActionMode) {
        super.onSupportActionModeFinished(mode)
        fragment.setBottomNavVisible(true)
    }

    override fun updateToolbarElevation(elevate: Boolean) {
        if (elevate) {
            setToolbarElevationDefault()
        } else {
            clearToolbarElevation()
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        setIntent(intent)
        fragment.handleIntent(intent)
    }

    override fun onGoOffline() {
        fragment.onGoOffline()
    }

    override fun onGoOnline() {
        fragment.onGoOnline()
    }

    override fun onBackPressed() {
        if (fragment.onBackPressed()) {
            return
        }
        super.onBackPressed()
    }

    fun isCurrentFragmentSelected(f: Fragment): Boolean {
        return fragment.currentFragment === f
    }

    fun getToolbar(): Toolbar {
        return binding.mainToolbar
    }

    override fun onUnreadNotification() {
        fragment.updateNotificationDot(true)
    }

    private fun setToolbarElevationDefault() {
        binding.mainToolbar.elevation = DimenUtil.dpToPx(DimenUtil.getDimension(R.dimen.toolbar_default_elevation))
    }

    private fun clearToolbarElevation() {
        binding.mainToolbar.elevation = 0f
    }

    companion object {
        fun newIntent(context: Context): Intent {
            return Intent(context, MainActivity::class.java)
        }
    }
}
