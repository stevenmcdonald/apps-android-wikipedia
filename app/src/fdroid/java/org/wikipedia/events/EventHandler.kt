package org.wikipedia.events

import android.content.Context
import android.os.Bundle
import android.util.Log

class EventHandler (applicationContext: Context) {

    // private val TAG = "EventHandler_Stub"
    // TEMP
    private val TAG = "ENVOY_LOG_STUB"

    fun logEvent(eventTag: String, eventBundle: Bundle) {
        Log.d(TAG, "LOG STUB: " + eventTag)
    }
}