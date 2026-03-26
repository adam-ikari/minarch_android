# AGENTS.md - Agentic Coding Guidelines

This file provides guidelines for AI agents working in this repository.

## Project Overview

MinArch Android is a libretro frontend for Android, based on MinUI. It uses:
- **Kotlin** for Android UI layer
- **C/JNI** for native libretro integration
- **Gradle** for build management
- **SDL2** (via NDK) for underlying graphics/audio

## Build Commands

### Gradle (Android)

```bash
# Build debug APK
./gradlew assembleDebug

# Build release APK
./gradlew assembleRelease

# Clean build
./gradlew clean

# Run lint (if configured)
./gradlew lint

# Run a specific task
./gradlew :app:assembleDebug
```

### NDK (Native Code)

```bash
# From project root
cd app/src/main/jni
$NDK_HOME/ndk-build

# Or with full path
/home/zhaodi-chen/android_sdk/cmdline-tools/latest/ndk/28.1.13356709/ndk-build -C app/src/main/jni
```

### Testing

This project uses Android's built-in testing. There are no explicit test targets configured.
- Unit tests can be added in `app/src/test/java/`
- Instrumented tests in `app/src/androidTest/java/`

To run tests:
```bash
./gradlew testDebugUnitTest
./gradlew connectedDebugAndroidTest
```

## Code Style Guidelines

### Kotlin

Follow standard Kotlin conventions (similar to Android Kotlin Style Guide):

1. **Imports**: Group in order: android, kotlin, java, project
   ```kotlin
   import android.app.Activity
   import android.os.Bundle
   import kotlinx.coroutines.CoroutineScope
   import java.io.File
   import com.minarch.databinding.ActivityMainBinding
   ```

2. **Naming**:
   - Classes: `PascalCase` (e.g., `MainActivity`, `CoreManager`)
   - Functions: `camelCase` (e.g., `setupVirtualGamepad()`)
   - Constants: `UPPER_SNAKE_CASE` in companion object (e.g., `REQUEST_OPEN_FILE`)
   - Variables: `camelCase` (e.g., `nativeLib`, `binding`)

3. **Types**: Use Kotlin's type system, avoid raw Java types
   - Use `Int`, `Boolean`, `String` not `int`, `boolean`, `String`
   - Use nullable types `?` appropriately: `var nativeLib: MinArchNative? = null`
   - Use `lateinit` for non-nullable vars initialized in `onCreate`

4. **Formatting**:
   - 4-space indentation
   - Max line length: 120 characters
   - Use expression bodies where appropriate: `fun isValid() = value > 0`

5. **Error Handling**:
   - Use `try-catch` for operations that may fail (file I/O, JNI calls)
   - Return `null` or use `Result<T>` for recoverable errors
   - Log errors with appropriate tag: `Log.e(TAG, "message", e)`

### Java/C (JNI/Native)

1. **Imports**: Standard C includes, then project headers
   ```c
   #include <jni.h>
   #include <android/log.h>
   #include "minarch/minarch_android.h"
   ```

2. **Naming**:
   - Functions: `snake_case` or `camelCase` per existing codebase
   - Macros/Constants: `UPPER_SNAKE_CASE`
   - Global variables: `g_` prefix (e.g., `g_core_handle`)

3. **Code Style**:
   - Use K&R brace style (same as existing code)
   - 4-space indentation
   - Comment complex logic
   - Avoid globals where possible; use static or pass context

4. **Error Handling**:
   - Return -1 or NULL on error
   - Log errors with `LOGE()` macro
   - Check return values of JNI functions

### XML (Layouts/Resources)

1. **Naming**: `snake_case` for IDs, `lowercase` for filenames
   ```xml
   android:id="@+id/btn_load_rom"
   android:id="@+id/menu_overlay"
   ```

2. **Attributes**: Order: id, layout width/height, other attributes

### Project-Specific Patterns

1. **JNI/Native Integration**:
   - JNI functions follow: `Java_com_package_ClassName_methodName`
   - Use `JNIEnv*` carefully; detach thread before returning
   - Wrap native calls in try-catch in Kotlin

2. **libretro Cores**:
   - Cores loaded via `dlopen()` from `/data/data/com.minarch/files/cores/`
   - Core .so files stored in `app/src/main/assets/cores/`
   - Naming convention: `<core>_libretro_android.so`

3. **CoreManager**:
   - Manages core loading/unloading
   - Built-in cores defined in `getBuiltInCores()`
   - Core assets copied to internal storage on first launch

4. **Graphics**:
   - Uses EGL + OpenGL ES 2.0 (not SDL2 for display)
   - Video callback: `retro_video_refresh_t`
   - Framebuffer handling in `minarch_android.c`

## Common Tasks

### Adding a New Core

1. Download core `.so` from https://buildbot.libretro.com/nightly/android/latest/arm64-v8a/
2. Place in `app/src/main/assets/cores/`
3. Add entry in `CoreManager.getBuiltInCores()`
4. Add filename mapping in `CoreManager.getCoreFileName()`

### Modifying Native Code

1. Edit C files in `app/src/main/jni/`
2. Test with: `ndk-build -C app/src/main/jni`
3. Rebuild APK: `./gradlew assembleDebug`

### Updating libretro API

The libretro header is in `app/src/main/jni/minarch/libretro.h`.
Update function pointer types and enum values as needed.

## Dependencies

- Android SDK 34 (compileSdk)
- NDK 28.1.13356709
- Kotlin 1.9.20
- AGP 8.2.0

## Key Files

| File | Purpose |
|------|---------|
| `MainActivity.kt` | Main UI entry point |
| `CoreManager.kt` | Core loading/management |
| `jni_entry.c` | JNI bridge to native code |
| `minarch_android.c` | libretro implementation |
| `Android.mk` | NDK build configuration |
| `build.gradle` | Android build configuration |