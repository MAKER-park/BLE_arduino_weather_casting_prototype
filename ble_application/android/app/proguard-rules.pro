# 기본적인 설정
-keep class * extends java.util.ListResourceBundle {
    protected Object[][] getContents();
}

-keep public class * extends android.app.backup.BackupAgentHelper
-keep public class * extends android.app.backup.BackupAgent

-keep public class com.google.android.gms.common.api.GoogleApiClient {
    public <init>(android.content.Context, com.google.android.gms.common.api.GoogleApiClient$Builder);
}

-keep public class * extends com.google.android.gms.common.api.GoogleApiClient {
    public <init>(android.content.Context, com.google.android.gms.common.api.GoogleApiClient$Builder);
}

-keep class * extends android.app.Service {
    public <init>();
}

# BLE 관련 설정 추가
-keep class no.nordicsemi.android.** { *; }
-keep class com.polidea.rxandroidble2.** { *; }
