package com.minarch

import android.content.Context
import android.content.SharedPreferences
import java.io.File

/**
 * Manages ROM files and game library
 */
class GameManager(private val context: Context) {

    data class GameInfo(
        val path: String,
        val name: String,
        val system: String,
        val core: String?,
        val lastPlayed: Long = 0,
        val playCount: Int = 0,
        val favorite: Boolean = false
    )

    companion object {
        private const val PREFS_NAME = "minarch_games"
        private const val KEY_RECENT = "recent_games"
        private const val MAX_RECENT = 20
    }

    private val prefs: SharedPreferences =
        context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)

    /**
     * Get recent games
     */
    fun getRecentGames(): List<GameInfo> {
        val recentPaths = prefs.getStringSet(KEY_RECENT, emptySet()) ?: emptySet()
        return recentPaths.mapNotNull { path ->
            val file = File(path)
            if (file.exists()) {
                GameInfo(
                    path = path,
                    name = file.nameWithoutExtension,
                    system = detectSystem(file.name),
                    core = null
                )
            } else null
        }.take(MAX_RECENT)
    }

    /**
     * Add to recent games
     */
    fun addToRecent(path: String) {
        val recent = prefs.getStringSet(KEY_RECENT, mutableSetOf())?.toMutableSet()
            ?: mutableSetOf()

        // Remove if exists (to move to front)
        recent.remove(path)
        recent.add(path)

        // Keep only MAX_RECENT
        while (recent.size > MAX_RECENT) {
            recent.remove(recent.first())
        }

        prefs.edit().putStringSet(KEY_RECENT, recent).apply()
    }

    /**
     * Clear recent games
     */
    fun clearRecent() {
        prefs.edit().remove(KEY_RECENT).apply()
    }

    /**
     * Scan directory for ROMs
     */
    fun scanDirectory(dir: File, extensions: List<String> = getAllExtensions()): List<GameInfo> {
        val games = mutableListOf<GameInfo>()

        if (!dir.exists() || !dir.isDirectory) return games

        dir.listFiles()?.forEach { file ->
            if (file.isFile) {
                val ext = file.extension.lowercase()
                if (extensions.contains(ext)) {
                    games.add(GameInfo(
                        path = file.absolutePath,
                        name = cleanGameName(file.nameWithoutExtension),
                        system = detectSystem(file.name),
                        core = null
                    ))
                }
            } else if (file.isDirectory) {
                games.addAll(scanDirectory(file, extensions))
            }
        }

        return games.sortedBy { it.name }
    }

    /**
     * Get all supported extensions
     */
    fun getAllExtensions(): List<String> {
        return listOf(
            // NES
            "nes", "nez", "unf", "unif",
            // Game Boy
            "gb", "gbc",
            // GBA
            "gba", "agb",
            // SNES
            "sfc", "smc", "swc",
            // Genesis
            "md", "gen", "bin",
            // SMS
            "sms",
            // GG
            "gg",
            // PCE
            "pce",
            // N64
            "n64", "z64", "v64",
            // NDS
            "nds",
            // PSX
            "bin", "cue", "iso", "img",
            // VB
            "vb", "vboy",
            // Lynx
            "lnx",
            // Jaguar
            "jag",
            // Atari 2600
            "a26",
            // Dreamcast
            "gdi",
            // P8
            "p8"
        )
    }

    /**
     * Detect system from filename
     */
    fun detectSystem(fileName: String): String {
        val lower = fileName.lowercase()
        return when {
            lower.endsWith(".nes") || lower.endsWith(".nez") -> "NES"
            lower.endsWith(".gb") -> "GB"
            lower.endsWith(".gbc") -> "GBC"
            lower.endsWith(".gba") -> "GBA"
            lower.endsWith(".sfc") || lower.endsWith(".smc") -> "SNES"
            lower.endsWith(".md") || lower.endsWith(".gen") || lower.endsWith(".bin") -> "Genesis"
            lower.endsWith(".sms") -> "Master System"
            lower.endsWith(".gg") -> "Game Gear"
            lower.endsWith(".pce") -> "TurboGrafx-16"
            lower.endsWith(".n64") || lower.endsWith(".z64") || lower.endsWith(".v64") -> "N64"
            lower.endsWith(".nds") -> "NDS"
            lower.endsWith(".bin") || lower.endsWith(".iso") -> "PlayStation"
            lower.endsWith(".vb") || lower.endsWith(".vboy") -> "Virtual Boy"
            lower.endsWith(".lnx") -> "Atari Lynx"
            lower.endsWith(".jag") -> "Atari Jaguar"
            lower.endsWith(".a26") -> "Atari 2600"
            lower.endsWith(".p8") -> "Pico-8"
            else -> "Unknown"
        }
    }

    /**
     * Clean game name (remove region, etc)
     */
    private fun cleanGameName(name: String): String {
        var cleaned = name

        // Remove common suffixes
        val patterns = listOf(
            Regex("\\(.*?\\)"),  // (USA)
            Regex("\\[.*?\\]"),  // [!]
            Regex("\\{.*?}"),   // {C}
            Regex("\\s+\\d+$"), // 1, 2, 3
        )

        patterns.forEach { pattern ->
            cleaned = pattern.replace(cleaned, "")
        }

        // Replace underscores with spaces
        cleaned = cleaned.replace("_", " ")

        return cleaned.trim()
    }

    /**
     * Get system icon resource
     */
    fun getSystemIcon(system: String): Int {
        return when (system) {
            "NES" -> R.drawable.ic_console_nes
            "GB", "GBC" -> R.drawable.ic_console_gb
            "GBA" -> R.drawable.ic_console_gba
            "SNES" -> R.drawable.ic_console_snes
            "Genesis", "Master System" -> R.drawable.ic_console_genesis
            "PlayStation" -> R.drawable.ic_console_psx
            "N64" -> R.drawable.ic_console_n64
            "NDS" -> R.drawable.ic_console_nds
            else -> R.drawable.ic_console_generic
        }
    }
}