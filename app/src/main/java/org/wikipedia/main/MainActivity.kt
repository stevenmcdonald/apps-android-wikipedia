package org.wikipedia.main

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.net.Uri
import android.os.Bundle
import android.util.Log
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.view.ActionMode
import androidx.appcompat.widget.Toolbar
import androidx.fragment.app.Fragment
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import org.greatfire.envoy.*
import org.greatfire.wikiunblocked.Secrets
import org.wikipedia.Constants
import org.wikipedia.R
import org.wikipedia.activity.SingleFragmentActivity
import org.wikipedia.databinding.ActivityMainBinding
import org.wikipedia.dataclient.WikiSite
import org.wikipedia.events.EventHandler
import org.wikipedia.navtab.NavTab
import org.wikipedia.onboarding.InitialOnboardingActivity
import org.wikipedia.page.PageActivity
import org.wikipedia.settings.Prefs
import org.wikipedia.util.DimenUtil
import org.wikipedia.util.FeedbackUtil
import org.wikipedia.util.ResourceUtil

class MainActivity : SingleFragmentActivity<MainFragment>(), MainFragment.Callback {

    private val TAG = "MainActivity"

    private val DIRECT_URL = arrayListOf<String>() // ("https://www.wikipedia.org/")

    // event logging
    private var eventHandler: EventHandler? = null
    private val EVENT_TAG_DIRECT = "DIRECT_URL"
    private val EVENT_PARAM_DIRECT_URL = "direct_url_value"
    private val EVENT_PARAM_DIRECT_SERVICE = "direct_url_service"
    private val EVENT_TAG_SELECT = "SELECTED_URL"
    private val EVENT_PARAM_SELECT_URL = "selected_url_value"
    private val EVENT_PARAM_SELECT_SERVICE = "selected_url_service"
    private val EVENT_TAG_VALID = "VALID_URL"
    private val EVENT_PARAM_VALID_URL = "valid_url_value"
    private val EVENT_PARAM_VALID_SERVICE = "valid_url_service"
    private val EVENT_TAG_INVALID = "INVALID_URL"
    private val EVENT_PARAM_INVALID_URL = "invalid_url_value"
    private val EVENT_PARAM_INVALID_SERVICE = "invalid_url_service"

    private lateinit var binding: ActivityMainBinding

    private var controlNavTabInFragment = false
    private val onboardingLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { }

    // add all string values to this list value
    private val listOfUrls = mutableListOf<String>()
    private val invalidUrls = mutableListOf<String>()

    // TEMP - for debug menu
    private val validServices = mutableListOf<String>()
    private val invalidServices = mutableListOf<String>()

    private var waitingForEnvoy = false

    // this receiver should be triggered by a success or failure broadcast from either the
    // NetworkIntentService (indicating whether submitted urls were valid or invalid) or the
    // ShadowsocksService (indicating whether the service was successfully started or not
    private val mBroadcastReceiver: BroadcastReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            if (intent != null && context != null) {
                if (intent.action == ENVOY_BROADCAST_VALIDATION_SUCCEEDED) {
                    val validUrl = intent.getStringExtra(ENVOY_DATA_URL_SUCCEEDED)
                    val validService = intent.getStringExtra(ENVOY_DATA_SERVICE_SUCCEEDED)
                    val validStrategy = intent.getIntExtra(ENVOY_DATA_STRATEGY_SUCCEEDED, 0)

                    if (!validService.isNullOrEmpty()) {
                        validServices.add(validService + "/" + validStrategy + " - " + validUrl)
                        Prefs.validServices = validServices
                    }

                    if (validUrl.isNullOrEmpty()) {
                        Log.e(TAG, "received a valid url that was empty or null")
                    } else if (waitingForEnvoy) {
                        waitingForEnvoy = false
                        Log.d(TAG, "received a valid url with strategy " + validStrategy)
                        // select the first url that is returned (assumed to have the lowest latency)
                        if (DIRECT_URL.contains(validUrl)) {

                            val bundle = Bundle()
                            bundle.putString(EVENT_PARAM_DIRECT_URL, validUrl)
                            bundle.putString(EVENT_PARAM_DIRECT_SERVICE, validService)
                            eventHandler?.logEvent(EVENT_TAG_DIRECT, bundle)

                            Log.d(TAG, "got direct url: " + validUrl + ", don't need to start engine")
                        } else {

                            val bundle = Bundle()
                            bundle.putString(EVENT_PARAM_SELECT_URL, validUrl)
                            bundle.putString(EVENT_PARAM_SELECT_SERVICE, validService)
                            eventHandler?.logEvent(EVENT_TAG_SELECT, bundle)

                            Log.d(TAG, "found a valid url: " + validUrl + ", start engine")
                            CronetNetworking.initializeCronetEngine(context, validUrl, false, validStrategy)

                            if (fragment is MainFragment) {
                                Log.d(TAG, "engine started, refresh main fragment")
                                fragment.refreshFragment()
                            } else {
                                Log.d(TAG, "unexpected fragment class, can't refresh")
                            }
                        }
                    } else {

                        val bundle = Bundle()
                        bundle.putString(EVENT_PARAM_VALID_URL, validUrl)
                        bundle.putString(EVENT_PARAM_VALID_SERVICE, validService)
                        eventHandler?.logEvent(EVENT_TAG_VALID, bundle)

                        Log.d(TAG, "already selected a valid url, ignore valid url: " + validUrl)
                    }
                } else if (intent.action == ENVOY_BROADCAST_VALIDATION_FAILED) {
                    val invalidUrl = intent.getStringExtra(ENVOY_DATA_URL_FAILED)
                    val invalidService = intent.getStringExtra(ENVOY_DATA_SERVICE_FAILED)
                    val invalidStrategy = intent.getIntExtra(ENVOY_DATA_STRATEGY_FAILED, 0)

                    if (!invalidService.isNullOrEmpty()) {
                        invalidServices.add(invalidService + "/" + invalidStrategy + " - " + invalidUrl)
                        Prefs.invalidServices = invalidServices
                    }

                    if (invalidUrl.isNullOrEmpty()) {
                        Log.e(TAG, "received an invalid url that was empty or null")
                    } else {

                        val bundle = Bundle()
                        bundle.putString(EVENT_PARAM_INVALID_URL, invalidUrl)
                        bundle.putString(EVENT_PARAM_INVALID_SERVICE, invalidService)
                        eventHandler?.logEvent(EVENT_TAG_INVALID, bundle)

                        Log.d(TAG, "got invalid url: " + invalidUrl)
                        invalidUrls.add(invalidUrl)
                        // TODO: there isn't an obvious way to check unchecked/invalid counts when getting new urls from dnstt
                        if (waitingForEnvoy && (invalidUrls.size >= listOfUrls.size)) {
                            Log.e(TAG, "no urls left to try, cannot start envoy/cronet")
                            // TEMP: clearing this flag will cause any dnstt urls that follow to be ignored
                            waitingForEnvoy = false
                        } else {
                            Log.e(TAG, "still trying urls, " + invalidUrls.size + " out of " + listOfUrls.size + " failed")
                        }
                    }
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

    fun envoyInit() {

        listOfUrls.clear()
        invalidUrls.clear()

        validServices.clear()
        Prefs.validServices = validServices
        invalidServices.clear()
        Prefs.invalidServices = invalidServices

        // secrets don't support fdroid package name
        val shortPackage = packageName.removeSuffix(".fdroid")

        /* expected format:
               0. dnstt domain
               1. dnstt key
               2. dnstt path
               3. doh url
               4. dot address
               (either 4 or 5 should be an empty string) */
        val dnsttConfig = mutableListOf<String>()
//        dnsttConfig.add(Secrets().getdnsttdomain(shortPackage))
//        dnsttConfig.add(Secrets().getdnsttkey(shortPackage))
//        dnsttConfig.add(Secrets().getdnsttpath(shortPackage))
//        dnsttConfig.add(Secrets().getdohUrl(shortPackage))
//        dnsttConfig.add(Secrets().getdotAddr(shortPackage))

        if (Secrets().getdefProxy(shortPackage).isNullOrEmpty()) {
            Log.w(TAG, "no default proxy urls found, submit empty list to check dnstt for urls")
        } else {
            Log.d(TAG, "found default proxy urls: " + Secrets().getdefProxy(shortPackage))
            listOfUrls.addAll(Secrets().getdefProxy(shortPackage).split(","))
        }

        NetworkIntentService.submit(
            this@MainActivity,
            listOfUrls,
            DIRECT_URL,
            Secrets().gethystCert(shortPackage),
            dnsttConfig
        )
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // firebase logging
        if (Prefs.isFirebaseLoggingEnabled) {
            eventHandler = EventHandler(applicationContext)
        } else {
            Log.d("ENVOY_LOG", "firebase logging off, don't initialize firebase")
            eventHandler = null
        }

        setImageZoomHelper()
        if (Prefs.isInitialOnboardingEnabled && savedInstanceState == null && !intent.hasExtra(Constants.INTENT_EXTRA_IMPORT_READING_LISTS)) {
            // Updating preference so the search multilingual tooltip
            // is not shown again for first time users
            Prefs.isMultilingualSearchTooltipShown = false

            // Use startActivityForResult to avoid preload the Feed contents before finishing the initial onboarding.
            onboardingLauncher.launch(InitialOnboardingActivity.newIntent(this))
        }
        setNavigationBarColor(ResourceUtil.getThemedColor(this, R.attr.nav_tab_background_color))
        setSupportActionBar(binding.mainToolbar)
        supportActionBar?.title = ""
        supportActionBar?.setDisplayHomeAsUpEnabled(false)
        binding.mainToolbar.navigationIcon = null

        if (savedInstanceState == null) {
            handleIntent(intent)
        }
    }

    override fun onStart() {
        super.onStart()

        // moved to start/stop to avoid an issue with registering multiple instances of the receiver when app is swiped away
        Log.d(TAG, "start/register broadcast receiver")
        // register to receive test results
        LocalBroadcastManager.getInstance(this).registerReceiver(mBroadcastReceiver, IntentFilter().apply {
            addAction(ENVOY_BROADCAST_VALIDATION_SUCCEEDED)
            addAction(ENVOY_BROADCAST_VALIDATION_FAILED)
        })
    }

    override fun onResume() {
        super.onResume()

        // start cronet here to prevent exception from starting a service when out of focus
        if (CronetNetworking.cronetEngine() != null) {
            Log.d(TAG, "cronet already running, don't try to start envoy again")
        } else if (waitingForEnvoy) {
            Log.d(TAG, "already processing urls, don't try to start envoy again")
        } else {
            // run envoy setup (fetches and validate urls)
            Log.d(TAG, "start envoy to process urls")
            waitingForEnvoy = true
            envoyInit()
        }

        invalidateOptionsMenu()
    }

    override fun onStop() {
        super.onStop()

        // moved to start/stop to avoid an issue with registering multiple instances of the receiver when app is swiped away
        Log.d(TAG, "stop/unregister broadcast receiver")
        // unregister receiver for test results
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mBroadcastReceiver)
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

    private fun handleIntent(intent: Intent) {
        if (Intent.ACTION_VIEW == intent.action && intent.data != null) {
            // TODO: handle special cases of non-article content, e.g. shared reading lists.
            intent.data?.let {
                if (it.authority.orEmpty().endsWith(WikiSite.BASE_DOMAIN)) {
                    // Pass it right along to PageActivity
                    val uri = Uri.parse(it.toString().replace("wikipedia://", WikiSite.DEFAULT_SCHEME + "://"))
                    startActivity(Intent(this, PageActivity::class.java)
                            .setAction(Intent.ACTION_VIEW)
                            .setData(uri))
                }
            }
        }
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
