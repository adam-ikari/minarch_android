# Add project specific ProGuard rules here.
-keep class com.minarch.** { *; }
-keepclassmembers class * {
    native <methods>;
}