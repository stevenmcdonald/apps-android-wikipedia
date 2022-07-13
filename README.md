### Wikipedia Android app

This repository is a fork of the official [Wikipedia Android app](https://play.google.com/store/apps/details?id=org.wikipedia) that has been integrated with the [Envoy library](https://github.com/greatfire/envoy).  There are several additional steps required to build this version of the app:

1. Download cronet.aar  
   Cronet.aar has the same API as Google's cronet (except for one extra method `setEnvoyUrl`).
   Download this aar file [cronet-0.0.87.0.4280.66.aar](https://repo1.maven.org/maven2/info/guardianproject/envoy/cronet/0.0.87.0.4280.66/cronet-0.0.87.0.4280.66.aar).

2. To build envoy-$BUILD.aar, follow the instructions [here](https://github.com/greatfire/envoy/tree/master/android) 
   Envoy is a bootstrapping library that has adapters for serval popular libraries (OkHttp/Retrofit, Volley, etc).
   Copy and rename the cronet-0.0.87.0.4280.66.aar file from step one to `android/cronet/cronet-$BUILD.aar`.
   $BUILD will be debug or release depending on your build variant.

3. Copy cronet-$BUILD.aar and envoy-$BUILD.aar to `/app/libs` then update `MainActivity` with a list of possible proxy urls and build the application.

### Documentation

Documentation for the Wikipedia app is kept on [the wiki](https://www.mediawiki.org/wiki/Wikimedia_Apps/Team/Android/App_hacking).

### Issues

Please file issues with the Wikipedia app in [the bug tracker][1]


[1]: https://phabricator.wikimedia.org/maniphest/task/edit/form/10/?title=&projects=wikipedia-android-app-backlog&points=1&description=%3D%3D%3D+Steps+to+reproduce%0A%23+%0A%23+%0A%23+%0A%0A%3D%3D%3D+Expected+results%0A%0A%3D%3D%3D+Actual+results%0A%0A%3D%3D%3D+Stack%20trace%0A%60%60%60lines%3D10%0A(Optional%20logcat%20output)%0A%60%60%60%0A%0A%3D%3D%3D+Environments+observed%0A**App+version%3A+**+%0A**Android+OS+versions%3A**+%0A**Device+model%3A**+%0A**Device+language%3A**
