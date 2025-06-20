package org.wikipedia.dataclient.okhttp

import android.os.Build
import okhttp3.Cache
import okhttp3.CacheControl
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import okhttp3.tls.HandshakeCertificates
import org.greatfire.envoy.EnvoyInterceptor
import org.wikipedia.R
import org.wikipedia.WikipediaApp
import org.wikipedia.dataclient.SharedPreferenceCookieManager
import org.wikipedia.settings.Prefs
import java.io.File
import java.security.cert.CertificateFactory
import java.security.cert.X509Certificate
import java.util.concurrent.TimeUnit

import health.flo.network.ohttp.client.IsOhttpEnabledProvider
import health.flo.network.ohttp.client.OhttpConfig
import health.flo.network.ohttp.client.setupOhttp
import okhttp3.HttpUrl.Companion.toHttpUrl

object OkHttpConnectionFactory {
    val CACHE_CONTROL_FORCE_NETWORK = CacheControl.Builder().maxAge(0, TimeUnit.SECONDS).build()
    val CACHE_CONTROL_MAX_STALE = CacheControl.Builder().maxStale(Int.MAX_VALUE, TimeUnit.SECONDS).build()
    val CACHE_CONTROL_NONE = CacheControl.Builder().build()

    private const val CACHE_DIR_NAME = "okhttp-cache"
    private const val NET_CACHE_SIZE = (64 * 1024 * 1024).toLong()
    private val NET_CACHE = Cache(File(WikipediaApp.instance.cacheDir, CACHE_DIR_NAME), NET_CACHE_SIZE)
    val client = createClient()

    private fun createClient(): OkHttpClient {
        val configRequestsCache: Cache = Cache(
            directory = File(WikipediaApp.instance.cacheDir, "ohttp"),
            maxSize = 50L * 1024L * 1024L // 50 MiB
        )

        // provide your IsOhttpEnabledProvider implementation if you need to enable/disable OHTTP in runtime
        val isOhttpEnabled: IsOhttpEnabledProvider = IsOhttpEnabledProvider { true }

        val ohttpConfig = OhttpConfig(
            relayUrl = "https://proxy-test.unready.im/".toHttpUrl(), // relay server
            userAgent = "Minimal User Agent", // user agent for OHTTP requests to the relay server
            configServerConfig = OhttpConfig.ConfigServerConfig(
                configUrl = "https://gateway-test.unready.im/ohttp-keys".toHttpUrl(), // crypto config
                configCache = configRequestsCache,
            ),
        )

        val builder = OkHttpClient.Builder()
            .cookieJar(SharedPreferenceCookieManager.instance)
            .cache(NET_CACHE)
            .readTimeout(20, TimeUnit.SECONDS)
            .addInterceptor(UnsuccessfulResponseInterceptor())
            .addNetworkInterceptor(CacheControlInterceptor())
            .addInterceptor(CommonHeaderRequestInterceptor())
            .addInterceptor(DefaultMaxStaleRequestInterceptor())
            .addInterceptor(OfflineCacheInterceptor())
            .addInterceptor(TestStubInterceptor())
            .addInterceptor(TitleEncodeInterceptor())
            .addInterceptor(HttpLoggingInterceptor().setLevel(Prefs.retrofitLogLevel))
            // this interceptor will be bypassed if no valid proxy urls were found at startup
            // the app will connect to the internet directly if possible
//            .addInterceptor(CronetInterceptor())
//            .addInterceptor(EnvoyInterceptor())


//        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
//            val certFactory = CertificateFactory.getInstance("X.509")
//            val certificates = HandshakeCertificates.Builder()
//                .addPlatformTrustedCertificates()
//                .addTrustedCertificate(certFactory.generateCertificate(WikipediaApp.instance.resources.openRawResource(R.raw.isrg_root_x1)) as X509Certificate)
//                .addTrustedCertificate(certFactory.generateCertificate(WikipediaApp.instance.resources.openRawResource(R.raw.isrg_root_x2)) as X509Certificate)
//                .build()
//            builder.sslSocketFactory(certificates.sslSocketFactory(), certificates.trustManager)
//        }
        // this runs build()
        return builder.setupOhttp( // setup OHTTP as the final step
                config=ohttpConfig,
                isOhttpEnabled = isOhttpEnabled,
            )

//        return builder.build()
    }
}
