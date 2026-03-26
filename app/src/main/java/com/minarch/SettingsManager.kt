package com.minarch

import android.content.Context
import android.content.SharedPreferences

/**
 * User settings manager
 */
class SettingsManager(context: Context) {

    companion object {
        private const val PREFS_NAME = "minarch_settings"

        // Settings keys
        const val KEY_SHOW_VIRTUAL_CONTROLS = "show_virtual_controls"
        const val KEY_SHOW_FPS = "show_fps"
        const val KEY_SKIP_FRAMES = "skip_frames"
        const val KEY_VOLUME = "volume"
        const val KEY_SCREEN_SCALING = "screen_scaling"
        const val KEY_SCREEN_EFFECT = "screen_effect"
        const val KEY_AUTOSAVE = "autosave"
        const val KEY_FAST_FORWARD = "fast_forward"
        const val KEY_OVERCLOCK = "overclock"
        const val KEY_LAST_ROMS_PATH = "last_roms_path"
        const val KEY_LAST_CORE_PATH = "last_core_path"

        // Scaling options
        const val SCALE_NATIVE = 0
        const val SCALE_ASPECT = 1
        const val SCALE_FULLSCREEN = 2
        const val SCALE_CROPPED = 3

        // Effect options
        const val EFFECT_NONE = 0
        const val EFFECT_SCANLINE = 1
        const val EFFECT_RETRO = 2
    }

    private val prefs: SharedPreferences =
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

    // Virtual controls
    var showVirtualControls: Boolean
        get() = prefs.getBoolean(KEY_SHOW_VIRTUAL_CONTROLS, true)
        set(value) = prefs.edit().putBoolean(KEY_SHOW_VIRTUAL_CONTROLS, value).apply()

    // FPS display
    var showFps: Boolean
        get() = prefs.getBoolean(KEY_SHOW_FPS, false)
        set(value) = prefs.edit().putBoolean(KEY_SHOW_FPS, value).apply()

    // Frame skipping
    var skipFrames: Int
        get() = prefs.getInt(KEY_SKIP_FRAMES, 0)
        set(value) = prefs.edit().putInt(KEY_SKIP_FRAMES, value).apply()

    // Volume
    var volume: Int
        get() = prefs.getInt(KEY_VOLUME, 100)
        set(value) = prefs.edit().putInt(KEY_VOLUME, value.coerceIn(0, 100)).apply()

    // Screen scaling
    var screenScaling: Int
        get() = prefs.getInt(KEY_SCREEN_SCALING, SCALE_ASPECT)
        set(value) = prefs.edit().putInt(KEY_SCREEN_SCALING, value).apply()

    // Screen effect
    var screenEffect: Int
        get() = prefs.getInt(KEY_SCREEN_EFFECT, EFFECT_NONE)
        set(value) = prefs.edit().putInt(KEY_SCREEN_EFFECT, value).apply()

    // Autosave
    var autosave: Boolean
        get() = prefs.getBoolean(KEY_AUTOSAVE, true)
        set(value) = prefs.edit().putBoolean(KEY_AUTOSAVE, value).apply()

    // Fast forward
    var fastForward: Boolean
        get() = prefs.getBoolean(KEY_FAST_FORWARD, false)
        set(value) = prefs.edit().putBoolean(KEY_FAST_FORWARD, value).apply()

    // Overclock
    var overclock: Int
        get() = prefs.getInt(KEY_OVERCLOCK, 1)
        set(value) = prefs.edit().putInt(KEY_OVERCLOCK, value).apply()

    // Last paths
    var lastRomsPath: String?
        get() = prefs.getString(KEY_LAST_ROMS_PATH, null)
        set(value) = prefs.edit().putString(KEY_LAST_ROMS_PATH, value).apply()

    var lastCorePath: String?
        get() = prefs.getString(KEY_LAST_CORE_PATH, null)
        set(value) = prefs.edit().putString(KEY_LAST_CORE_PATH, value).apply()

    /**
     * Get scaling mode name
     */
    fun getScalingName(mode: Int): String {
        return when (mode) {
            SCALE_NATIVE -> "Native"
            SCALE_ASPECT -> "Aspect Ratio"
            SCALE_FULLSCREEN -> "Fullscreen"
            SCALE_CROPPED -> "Cropped"
            else -> "Unknown"
        }
    }

    /**
     * Get effect name
     */
    fun getEffectName(effect: Int): String {
        return when (effect) {
            EFFECT_NONE -> "None"
            EFFECT_SCANLINE -> "Scanlines"
            EFFECT_RETRO -> "Retro"
            else -> "Unknown"
        }
    }

    /**
     * Get all settings as map
     */
    fun getAllSettings(): Map<String, Any> {
        return mapOf(
            KEY_SHOW_VIRTUAL_CONTROLS to showVirtualControls,
            KEY_SHOW_FPS to showFps,
            KEY_SKIP_FRAMES to skipFrames,
            KEY_VOLUME to volume,
            KEY_SCREEN_SCALING to screenScaling,
            KEY_SCREEN_EFFECT to screenEffect,
            KEY_AUTOSAVE to autosave,
            KEY_FAST_FORWARD to fastForward,
            KEY_OVERCLOCK to overclock
        )
    }

    /**
     * Reset to defaults
     */
    fun resetToDefaults() {
        prefs.edit().clear().apply()
    }
}