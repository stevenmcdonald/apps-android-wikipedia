### Wikipedia Android app

This repository is a fork of the official [Wikipedia Android app](https://play.google.com/store/apps/details?id=org.wikipedia) that has been integrated with the [Envoy library](https://github.com/greatfire/envoy).  There are several additional steps required to build this version of the app:

1. Download cronet.aar  
   Cronet.aar has the same API as Google's cronet (except for one extra method `setEnvoyUrl`).
   Download this aar file [cronet-0.0.87.0.4280.66.aar](https://repo1.maven.org/maven2/info/guardianproject/envoy/cronet/0.0.87.0.4280.66/cronet-0.0.87.0.4280.66.aar).

2. To build envoy-$BUILD.aar, follow the instructions in the android directory [here](https://github.com/mnbogner/envoy/tree/envoy-feedback)
   Envoy is a bootstrapping library that has adapters for serval popular libraries (OkHttp/Retrofit, Volley, etc).
   This version of the wikipedia app relies on broadcasts from the branch of Envoy specified above.
   Copy and rename the cronet-0.0.87.0.4280.66.aar file from step one to `android/cronet/cronet-$BUILD.aar`.
   $BUILD will be debug or release depending on your build variant.
   
3. Build IPtProxy from the branch [here](https://gitlab.com/stevenmcdonald/IPtProxy/-/tree/dnstt-client), or download the IPtProxy.aar file.

4. Copy cronet-$BUILD.aar, envoy-$BUILD.aar, and IPtProxy.aar to `/app/libs`.

5. Several project properties must be specified to identify an endpoint from which to download metadata.  These properties may be specified either on the command line or in Android Studio:
   - DNSTT server (-Pdnsttserver)
   - DNSTT key (-Pdnsttkey)
   - DNSTT path (-Pdnsttpath)
   - DOH url OR DOT address (-PdohUrl OR -PdotAddr)
   - Default proxy url to use if no metadata is downloaded (-PdefProxy)

![properties](https://user-images.githubusercontent.com/6945405/173699019-d023331e-9217-49b6-a88b-ca8afa40ce2a.png)

6. Specify the `greatfire` flavor when building the application, eiither on the command line or in Android Studio, e.g. `./gradlew assembleGreatfireDebug` or:

![variant](https://user-images.githubusercontent.com/6945405/173699837-5108cc88-4fe1-4165-9961-1d600e0f681c.png)

7. When running the application, click on the "More" icon a the bottom and look for the "Anonymous" icon.  A check indicates that Envoy is running, an X indicates that Envoy is not running.

![screen_mark](https://user-images.githubusercontent.com/6945405/173699843-3c9a50fd-3936-49ef-95f0-eb39dedea2bd.png)

### Documentation

Documentation for the Wikipedia app is kept on [the wiki](https://www.mediawiki.org/wiki/Wikimedia_Apps/Team/Android/App_hacking).

### Issues

Please file issues with the Wiki Unblocked app in [the bug tracker](https://github.com/greatfire/apps-android-wikipedia-envoy/issues).
