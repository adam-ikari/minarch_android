package com.minarch

import android.content.Context
import android.content.SharedPreferences
import java.io.File

/**
 * Manages libretro cores
 */
class CoreManager(private val context: Context) {

    data class CoreInfo(
        val fileName: String,
        val name: String,
        val extensions: String,
        val description: String
    )

    companion object {
        private const val PREFS_NAME = "minarch_cores"
        private const val KEY_LAST_CORE = "last_core"

        // Known core extensions and their systems
        val CORE_EXTENSIONS = mapOf(
            "so" to "libretro"
        )
    }

    private val prefs: SharedPreferences =
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

    private val coresDir: File by lazy {
        File(context.filesDir, "cores").also { it.mkdirs() }
    }

    private val coresCache = mutableListOf<CoreInfo>()

    /**
     * Get all available cores
     */
    fun getAvailableCores(): List<CoreInfo> {
        if (coresCache.isNotEmpty()) return coresCache

        val cores = mutableListOf<CoreInfo>()

        // Scan cores directory
        coresDir.listFiles()?.filter { it.extension == "so" }?.forEach { file ->
            cores.add(CoreInfo(
                fileName = file.name,
                name = file.nameWithoutExtension.removeSuffix("_libretro_android"),
                extensions = "*",
                description = "Custom core"
            ))
        }

        // Add built-in cores info (cores that would be bundled)
        getBuiltInCores().forEach { core ->
            if (cores.none { it.name == core.name }) {
                cores.add(core)
            }
        }

        coresCache.clear()
        coresCache.addAll(cores)
        return cores
    }

    /**
     * Get built-in cores info
     */
    private fun getBuiltInCores(): List<CoreInfo> {
        return listOf(
            CoreInfo("fceumm_libretro_android", "fceumm", "nes|nez|unf", "Nintendo (NES) / Famicom"),
            CoreInfo("gambatte_libretro_android", "gambatte", "gb|gbc", "Game Boy / Game Boy Color"),
            CoreInfo("mgba_libretro_android", "mgba", "gba|agb", "Game Boy Advance"),
            CoreInfo("snes9x_libretro_android", "snes9x", "sfc|smc|swc", "Super Nintendo"),
            CoreInfo("genesis_plus_gx_libretro_android", "genesis_plus_gx", "md|gen|sms|gg", "Sega Genesis / Master System"),
            CoreInfo("pcsx_rearmed_libretro_android", "pcsx_rearmed", "bin|cue|iso|img", "PlayStation"),
            CoreInfo("picodrive_libretro_android", "picodrive", "md|gen|bin|sms", "Sega Mega Drive / Master System"),
            CoreInfo("mednafen_pce_fast_libretro_android", "mednafen_pce_fast", "pce|iso", "TurboGrafx-16")
        )
    }

    /**
     * Get core file name from asset
     */
    fun getCoreFileName(coreName: String): String {
        return when (coreName) {
            "fceumm" -> "fceumm_libretro_android.so"
            "gambatte" -> "gambatte_libretro_android.so"
            "mgba" -> "mgba_libretro_android.so"
            "snes9x" -> "snes9x_libretro_android.so"
            "genesis_plus_gx" -> "genesis_plus_gx_libretro_android.so"
            "pcsx_rearmed" -> "pcsx_rearmed_libretro_android.so"
            "picodrive" -> "picodrive_libretro_android.so"
            "mednafen_pce_fast" -> "mednafen_pce_fast_libretro_android.so"
            else -> "$coreName.so"
        }
    }

    /**
     * Get core by name
     */
    fun getCore(name: String): CoreInfo? {
        return getAvailableCores().find { it.name == name }
    }

    /**
     * Get last used core
     */
    fun getLastCore(): CoreInfo? {
        val lastName = prefs.getString(KEY_LAST_CORE, null) ?: return null
        return getCore(lastName)
    }

    /**
     * Save last used core
     */
    fun setLastCore(name: String) {
        prefs.edit().putString(KEY_LAST_CORE, name).apply()
    }

    /**
     * Get cores for a specific file extension
     */
    fun getCoresForExtension(ext: String): List<CoreInfo> {
        val lowerExt = ext.lowercase()
        return getAvailableCores().filter { core ->
            core.extensions.split("|").any { it.equals(lowerExt, ignoreCase = true) }
        }
    }

    /**
     * Detect system from file extension
     */
    fun detectSystem(fileName: String): String {
        val ext = fileName.substringAfterLast(".", "").lowercase()
        return when (ext) {
            "nes", "nez", "unf" -> "NES"
            "gb", "gbc" -> "GB"
            "gba", "agb" -> "GBA"
            "sfc", "smc", "swc" -> "SNES"
            "md", "gen" -> "Genesis"
            "sms" -> "Master System"
            "gg" -> "Game Gear"
            "bin", "cue", "iso", "img" -> "PSX"
            "pce" -> "TurboGrafx-16"
            "vb", "vboy" -> "Virtual Boy"
            "n64", "z64", "v64" -> "N64"
            "nds" -> "NDS"
            "3ds", "cia" -> "3DS"
            "ws", "wsc" -> "Wonderswan"
            "ngp", "ngpc" -> "Neo Geo Pocket"
            "pkm", "min" -> "Pokémon Mini"
            "app" -> "Apple II"
            "lnx" -> "Atari Lynx"
            "jag" -> "Atari Jaguar"
            "a26" -> "Atari 2600"
            "a78" -> "Atari 7800"
            "col" -> "ColecoVision"
            "dff", "dfp" -> "Dreamcast"
            "gcm", "iso", "wbfs" -> "GameCube"
            "wad" -> "Wii"
            "nsp", "xci" -> "Switch"
            else -> "Unknown"
        }
    }

    /**
     * Install a core from assets
     */
    fun installCoreFromAssets(assetName: String): Boolean {
        return try {
            context.assets.open("cores/$assetName").use { input ->
                File(coresDir, assetName).outputStream().use { output ->
                    input.copyTo(output)
                }
            }
            coresCache.clear() // Refresh cache
            true
        } catch (e: Exception) {
            false
        }
    }
}