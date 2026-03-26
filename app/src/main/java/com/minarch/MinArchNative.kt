package com.minarch

import android.view.Surface
import java.util.concurrent.atomic.AtomicBoolean

class MinArchNative {

    companion object {
        init {
            System.loadLibrary("minarch")
        }
    }

    private val isRunning = AtomicBoolean(false)

    // Native methods
    external fun surfaceCreated(surface: Surface)
    external fun surfaceChanged(surface: Surface, width: Int, height: Int)
    external fun surfaceDestroyed()
    external fun loadRom(path: String): Boolean
    external fun loadCore(path: String): Boolean
    external fun saveState(): Boolean
    external fun loadState(): Boolean
    external fun resume()
    external fun pause()
    external fun destroy()
    external fun onKeyDown(keyCode: Int): Boolean
    external fun onKeyUp(keyCode: Int): Boolean
    external fun onJoystickMoved(axisValues: FloatArray): Boolean

    // Getters for native to call
    fun isRunning(): Boolean = isRunning.get()
    fun setRunning(running: Boolean) = isRunning.set(running)
}