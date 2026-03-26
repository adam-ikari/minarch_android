package com.minarch

import android.app.Activity
import android.content.Intent
import android.content.res.AssetManager
import android.os.Bundle
import android.view.SurfaceHolder
import android.view.View
import android.widget.Button
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.Toast
import com.minarch.databinding.ActivityMainBinding
import java.io.File

class MainActivity : Activity(), SurfaceHolder.Callback, VirtualGamepadView.InputListener {

    private lateinit var binding: ActivityMainBinding
    private var nativeLib: MinArchNative? = null
    private lateinit var virtualGamepad: VirtualGamepadView

    companion object {
        private const val REQUEST_OPEN_FILE = 1001
        private const val REQUEST_OPEN_CORE = 1002
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Initialize native library
        nativeLib = MinArchNative()

        // Setup virtual gamepad
        setupVirtualGamepad()

        setupSurface()
        setupUI()
        handleIntent(intent)

        // Copy assets to internal storage
        copyAssets()
    }

    private fun setupVirtualGamepad() {
        virtualGamepad = VirtualGamepadView(this)
        virtualGamepad.inputListener = this
        binding.container.addView(virtualGamepad, 0)
    }

    private fun setupSurface() {
        binding.gameSurface.holder.addCallback(this)
    }

    private fun setupUI() {
        binding.menuOverlay.visibility = View.VISIBLE

        binding.btnLoadRom.setOnClickListener {
            openFile("application/*", REQUEST_OPEN_FILE)
        }

        binding.btnLoadCore.setOnClickListener {
            openFile("application/octet-stream", REQUEST_OPEN_CORE)
        }

        binding.btnSaveState.setOnClickListener {
            nativeLib?.saveState()
        }

        binding.btnLoadState.setOnClickListener {
            nativeLib?.loadState()
        }

        binding.btnSettings.setOnClickListener {
            toggleMenu()
        }

        binding.btnExit.setOnClickListener {
            finish()
        }
    }

    private fun openFile(mimeType: String, requestCode: Int) {
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = mimeType
        }
        startActivityForResult(intent, requestCode)
    }

    private fun handleIntent(intent: Intent?) {
        intent?.data?.let { uri ->
            // Handle ROM file opened from file manager
            loadRomFromUri(uri)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode != Activity.RESULT_OK) return

        when (requestCode) {
            REQUEST_OPEN_FILE -> {
                data?.data?.let { uri ->
                    loadRomFromUri(uri)
                }
            }
            REQUEST_OPEN_CORE -> {
                data?.data?.let { uri ->
                    loadCoreFromUri(uri)
                }
            }
        }
    }

    private fun loadRomFromUri(uri: android.net.Uri) {
        showLoading(true)
        try {
            val path = copyToCache(uri, "rom")
            nativeLib?.loadRom(path)
            binding.menuOverlay.visibility = View.GONE
        } catch (e: Exception) {
            Toast.makeText(this, R.string.error_loading_rom, Toast.LENGTH_SHORT).show()
        }
        showLoading(false)
    }

    private fun loadCoreFromUri(uri: android.net.Uri) {
        showLoading(true)
        try {
            val path = copyToCache(uri, "core")
            nativeLib?.loadCore(path)
        } catch (e: Exception) {
            Toast.makeText(this, R.string.error_loading_core, Toast.LENGTH_SHORT).show()
        }
        showLoading(false)
    }

    private fun copyToCache(uri: android.net.Uri, suffix: String): String {
        val inputStream = contentResolver.openInputStream(uri)
        val file = File(cacheDir, "temp_$suffix")
        file.outputStream().use { output ->
            inputStream?.copyTo(output)
        }
        return file.absolutePath
    }

    private fun copyAssets() {
        val assetManager = assets
        try {
            val coresDir = File(filesDir, "cores")
            if (!coresDir.exists()) {
                coresDir.mkdirs()
            }

            // Copy default cores from assets if available
            val cores = assetManager.list("cores")
            cores?.forEach { core ->
                val destFile = File(coresDir, core)
                if (!destFile.exists()) {
                    assetManager.open("cores/$core").use { input ->
                        destFile.outputStream().use { output ->
                            input.copyTo(output)
                        }
                    }
                }
            }
        } catch (e: Exception) {
            // Assets may not exist, that's OK
        }
    }

    private fun toggleMenu() {
        binding.menuOverlay.visibility =
            if (binding.menuOverlay.visibility == View.VISIBLE) View.GONE
            else View.VISIBLE
    }

    private fun showLoading(show: Boolean) {
        binding.loading.visibility = if (show) View.VISIBLE else View.GONE
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        nativeLib?.surfaceCreated(holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        nativeLib?.surfaceChanged(holder.surface, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        nativeLib?.surfaceDestroyed()
    }

    override fun onResume() {
        super.onResume()
        nativeLib?.resume()
    }

    override fun onPause() {
        super.onPause()
        nativeLib?.pause()
    }

    override fun onDestroy() {
        nativeLib?.destroy()
        nativeLib = null
        super.onDestroy()
    }

    // Input handling
    override fun onKeyDown(keyCode: Int, event: android.view.KeyEvent?): Boolean {
        return nativeLib?.onKeyDown(keyCode) == true || super.onKeyDown(keyCode, event)
    }

    override fun onKeyUp(keyCode: Int, event: android.view.KeyEvent?): Boolean {
        return nativeLib?.onKeyUp(keyCode) == true || super.onKeyUp(keyCode, event)
    }

    override fun onGenericMotionEvent(event: android.view.MotionEvent?): Boolean {
        event?.let {
            if (it.source and android.view.InputDevice.SOURCE_JOYSTICK == android.view.InputDevice.SOURCE_JOYSTICK) {
                val axisX = it.getAxisValue(android.view.MotionEvent.AXIS_X)
                val axisY = it.getAxisValue(android.view.MotionEvent.AXIS_Y)
                return nativeLib?.onJoystickMoved(floatArrayOf(axisX, axisY)) == true
            }
        }
        return super.onGenericMotionEvent(event)
    }

    // VirtualGamepadView.InputListener
    override fun onButtonPressed(button: Int, pressed: Boolean) {
        if (pressed) {
            nativeLib?.onKeyDown(button)
        } else {
            nativeLib?.onKeyUp(button)
        }
    }

    override fun onJoystickMoved(axis: Int, value: Float) {
        // Handle analog joystick if needed
    }

    // Toggle virtual gamepad with menu button
    fun toggleVirtualGamepad() {
        virtualGamepad.toggle()
    }
}