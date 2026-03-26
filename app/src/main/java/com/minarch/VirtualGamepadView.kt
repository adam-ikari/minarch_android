package com.minarch

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.view.MotionEvent
import android.view.View
import kotlin.math.abs

/**
 * Virtual gamepad overlay for touch screen controls
 * Shows D-pad on left, action buttons on right
 */
class VirtualGamepadView(context: Context) : View(context) {

    interface InputListener {
        fun onButtonPressed(button: Int, pressed: Boolean)
        fun onJoystickMoved(axis: Int, value: Float)
    }

    var inputListener: InputListener? = null

    private val buttonPaint = Paint().apply {
        color = Color.argb(100, 255, 255, 255)
        style = Paint.Style.FILL
        isAntiAlias = true
    }

    private val buttonBorderPaint = Paint().apply {
        color = Color.argb(180, 255, 255, 255)
        style = Paint.Style.STROKE
        strokeWidth = 3f
        isAntiAlias = true
    }

    private val pressedPaint = Paint().apply {
        color = Color.argb(150, 0, 150, 255)
        style = Paint.Style.FILL
        isAntiAlias = true
    }

    // D-pad buttons (left side)
    private val dpadButtons = mutableMapOf<Int, RectF>()
    private val pressedDpad = mutableSetOf<Int>()

    // Action buttons (right side)
    private val actionButtons = mutableMapOf<Int, RectF>()
    private val pressedAction = mutableSetOf<Int>()

    // Button IDs
    companion object {
        const val BTN_UP = 0
        const val BTN_DOWN = 1
        const val BTN_LEFT = 2
        const val BTN_RIGHT = 3
        const val BTN_A = 4
        const val BTN_B = 5
        const val BTN_X = 6
        const val BTN_Y = 7
        const val BTN_START = 8
        const val BTN_SELECT = 9
    }

    private var dpadCenterX = 0f
    private var dpadCenterY = 0f
    private var actionCenterX = 0f
    private var actionCenterY = 0f
    private val buttonRadius = 40f
    private val buttonSpacing = 90f

    var showControls = true

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)

        // D-pad on left side
        dpadCenterX = w * 0.2f
        dpadCenterY = h * 0.7f

        // Action buttons on right side
        actionCenterX = w * 0.8f
        actionCenterY = h * 0.7f

        // D-pad layout (cross shape)
        dpadButtons.clear()
        dpadButtons[BTN_UP] = RectF(dpadCenterX - buttonRadius, dpadCenterY - buttonSpacing - buttonRadius,
                                      dpadCenterX + buttonRadius, dpadCenterY - buttonSpacing + buttonRadius)
        dpadButtons[BTN_DOWN] = RectF(dpadCenterX - buttonRadius, dpadCenterY + buttonSpacing - buttonRadius,
                                       dpadCenterX + buttonRadius, dpadCenterY + buttonSpacing + buttonRadius)
        dpadButtons[BTN_LEFT] = RectF(dpadCenterX - buttonSpacing - buttonRadius, dpadCenterY - buttonRadius,
                                       dpadCenterX - buttonSpacing + buttonRadius, dpadCenterY + buttonRadius)
        dpadButtons[BTN_RIGHT] = RectF(dpadCenterX + buttonSpacing - buttonRadius, dpadCenterY - buttonRadius,
                                        dpadCenterX + buttonSpacing + buttonRadius, dpadCenterY + buttonRadius)

        // Action buttons (diamond shape)
        actionButtons.clear()
        // A button (bottom)
        actionButtons[BTN_A] = RectF(actionCenterX + buttonSpacing * 0.7f - buttonRadius,
                                      actionCenterY + buttonSpacing * 0.5f - buttonRadius,
                                      actionCenterX + buttonSpacing * 0.7f + buttonRadius,
                                      actionCenterY + buttonSpacing * 0.5f + buttonRadius)
        // B button (right)
        actionButtons[BTN_B] = RectF(actionCenterX + buttonSpacing * 1.2f - buttonRadius,
                                      actionCenterY - buttonRadius,
                                      actionCenterX + buttonSpacing * 1.2f + buttonRadius,
                                      actionCenterY + buttonRadius)
        // X button (left)
        actionButtons[BTN_X] = RectF(actionCenterX + buttonSpacing * 0.2f - buttonRadius,
                                      actionCenterY - buttonRadius,
                                      actionCenterX + buttonSpacing * 0.2f + buttonRadius,
                                      actionCenterY + buttonRadius)
        // Y button (top)
        actionButtons[BTN_Y] = RectF(actionCenterX + buttonSpacing * 0.7f - buttonRadius,
                                      actionCenterY - buttonSpacing * 0.5f - buttonRadius,
                                      actionCenterX + buttonSpacing * 0.7f + buttonRadius,
                                      actionCenterY - buttonSpacing * 0.5f + buttonRadius)
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        if (!showControls) return

        // Draw D-pad
        for ((id, rect) in dpadButtons) {
            val paint = if (pressedDpad.contains(id)) pressedPaint else buttonPaint
            canvas.drawRoundRect(rect, 15f, 15f, paint)
            canvas.drawRoundRect(rect, 15f, 15f, buttonBorderPaint)
        }

        // Draw action buttons
        for ((id, rect) in actionButtons) {
            val paint = if (pressedAction.contains(id)) pressedPaint else buttonPaint
            canvas.drawRoundRect(rect, 15f, 15f, paint)
            canvas.drawRoundRect(rect, 15f, 15f, buttonBorderPaint)
        }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (!showControls) return false

        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE -> {
                // Check D-pad
                for ((id, rect) in dpadButtons) {
                    val contains = rect.contains(event.x, event.y)
                    if (contains && !pressedDpad.contains(id)) {
                        pressedDpad.add(id)
                        inputListener?.onButtonPressed(mapDpadButton(id), true)
                    } else if (!contains && pressedDpad.contains(id)) {
                        pressedDpad.remove(id)
                        inputListener?.onButtonPressed(mapDpadButton(id), false)
                    }
                }

                // Check action buttons
                for ((id, rect) in actionButtons) {
                    val contains = rect.contains(event.x, event.y)
                    if (contains && !pressedAction.contains(id)) {
                        pressedAction.add(id)
                        inputListener?.onButtonPressed(mapActionButton(id), true)
                    } else if (!contains && pressedAction.contains(id)) {
                        pressedAction.remove(id)
                        inputListener?.onButtonPressed(mapActionButton(id), false)
                    }
                }

                invalidate()
                return true
            }

            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                // Release all buttons
                for (id in pressedDpad.toList()) {
                    pressedDpad.remove(id)
                    inputListener?.onButtonPressed(mapDpadButton(id), false)
                }
                for (id in pressedAction.toList()) {
                    pressedAction.remove(id)
                    inputListener?.onButtonPressed(mapActionButton(id), false)
                }
                invalidate()
                return true
            }
        }

        return super.onTouchEvent(event)
    }

    private fun mapDpadButton(id: Int): Int {
        return when (id) {
            BTN_UP -> 19    // RETRO_DEVICE_ID_JOYPAD_UP
            BTN_DOWN -> 20  // RETRO_DEVICE_ID_JOYPAD_DOWN
            BTN_LEFT -> 21  // RETRO_DEVICE_ID_JOYPAD_LEFT
            BTN_RIGHT -> 22 // RETRO_DEVICE_ID_JOYPAD_RIGHT
            else -> -1
        }
    }

    private fun mapActionButton(id: Int): Int {
        return when (id) {
            BTN_A -> 96     // RETRO_DEVICE_ID_JOYPAD_A
            BTN_B -> 97     // RETRO_DEVICE_ID_JOYPAD_B
            BTN_X -> 99     // RETRO_DEVICE_ID_JOYPAD_X
            BTN_Y -> 100    // RETRO_DEVICE_ID_JOYPAD_Y
            else -> -1
        }
    }

    fun toggle() {
        showControls = !showControls
        invalidate()
    }
}