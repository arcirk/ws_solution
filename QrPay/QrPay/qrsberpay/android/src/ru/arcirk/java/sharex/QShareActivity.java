// (c) 2017 Ekkehard Gentz (ekke)
// this project is based on ideas from
// http://blog.lasconic.com/share-on-ios-and-android-using-qml/
// see github project https://github.com/lasconic/ShareUtils-QML
// also inspired by:
// https://www.androidcode.ninja/android-share-intent-example/
// https://www.calligra.org/blogs/sharing-with-qt-on-android/
// https://stackoverflow.com/questions/7156932/open-file-in-another-app
// http://www.qtcentre.org/threads/58668-How-to-use-QAndroidJniObject-for-intent-setData
// OpenURL in At Android: got ideas from:
// https://github.com/BernhardWenzel/open-url-in-qt-android
// https://github.com/tobiatesan/android_intents_qt
//
// see also /COPYRIGHT and /LICENSE

package ru.arcirk.java.sharex;

import org.qtproject.qt5.android.QtNative;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.*;
import android.content.*;
import android.app.*;

import java.lang.String;
import android.content.Intent;
import java.io.File;
import android.net.Uri;
import android.util.Log;
import android.content.ContentResolver;
import android.webkit.MimeTypeMap;

import ru.arcirk.utils.*;



public class QShareActivity extends QtActivity
{
    // native - must be implemented in Cpp via JNI
    // 'file' scheme or resolved from 'content' scheme:
    public static native void setFileUrlReceived(String url);
    // InputStream from 'content' scheme:
    public static native void setFileReceivedAndSaved(String url);
    //
    public static native void fireActivityResult(int requestCode, int resultCode);
    //
    public static native boolean checkFileExits(String url);

    public static boolean isIntentPending;
    public static boolean isInitialized;
    public static String workingDirPath;

    // Use a custom Chooser without providing own App as share target !
    // see QShareUtils.java createCustomChooserAndStartActivity()
    // Selecting your own App as target could cause AndroidOS to call
    // onCreate() instead of onNewIntent()
    // and then you are in trouble because we're using 'singleInstance' as LaunchMode
    // more details: my blog at Qt
    @Override
    public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
          Log.d("arcirk", "onCreate QShareActivity");
          // now we're checking if the App was started from another Android App via Intent
          Intent theIntent = getIntent();
          if (theIntent != null){
              String theAction = theIntent.getAction();
              if (theAction != null){
                  Log.d("arcirk onCreate ", theAction);
                  // QML UI not ready yet
                  // delay processIntent();
                  isIntentPending = true;
              }
          }
    } // onCreate

    // WIP - trying to find a solution to survive a 2nd onCreate
    // ongoing discussion in QtMob (Slack)
    // from other Apps not respecting that you only have a singleInstance
    // there are problems per ex. sharing a file from Google Files App,
    // but working well using Xiaomi FileManager App
    @Override
    public void onDestroy() {
        Log.d("arcirk", "onDestroy QShareActivity");
        // super.onDestroy();
        // System.exit() closes the App before doing onCreate() again
        // then the App was restarted, but looses context
        // This works for Samsung My Files
        // but Google Files doesn't call onDestroy()
        System.exit(0);
    }

    // we start Activity with result code
    // to test JNI with QAndroidActivityResultReceiver you must comment or rename
    // this method here - otherwise you'll get wrong request or result codes
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        Log.d("arcirk onActivityResult", "requestCode: "+requestCode);
        if (resultCode == RESULT_OK) {
            Log.d("arcirk onActivityResult - resultCode: ", "SUCCESS");
        } else {
            Log.d("arcirk onActivityResult - resultCode: ", "CANCEL");
        }
        // hint: result comes back too fast for Action SEND
        // if you want to delete/move the File add a Timer w 500ms delay
        // see Example App main.qml - delayDeleteTimer
        // if you want to revoke permissions for older OS
        // it makes sense also do this after the delay
        fireActivityResult(requestCode, resultCode);
    }

    // if we are opened from other apps:
    @Override
    public void onNewIntent(Intent intent) {
      Log.d("arcirk", "onNewIntent");
      super.onNewIntent(intent);
      setIntent(intent);
      // Intent will be processed, if all is initialized and Qt / QML can handle the event
      if(isInitialized) {
          processIntent();
      } else {
          isIntentPending = true;
      }
    } // onNewIntent

    public void checkPendingIntents(String workingDir) {
        isInitialized = true;
        workingDirPath = workingDir;
        Log.d("arcirk", workingDirPath);
        if(isIntentPending) {
            isIntentPending = false;
            Log.d("arcirk", "checkPendingIntents: true");
            processIntent();
        } else {
            Log.d("arcirk", "nothingPending");
        }
    } // checkPendingIntents

    // process the Intent if Action is SEND or VIEW
    private void processIntent(){
      Intent intent = getIntent();

      Uri intentUri;
      String intentScheme;
      String intentAction;
      // we are listening to android.intent.action.SEND or VIEW (see Manifest)
      if (intent.getAction().equals("android.intent.action.VIEW")){
             intentAction = "VIEW";
             intentUri = intent.getData();
      } else if (intent.getAction().equals("android.intent.action.SEND")){
             intentAction = "SEND";
              Bundle bundle = intent.getExtras();
              intentUri = (Uri)bundle.get(Intent.EXTRA_STREAM);
      } else {
              Log.d("arcirk Intent unknown action:", intent.getAction());
              return;
      }
      Log.d("arcirk action:", intentAction);
      if (intentUri == null){
            Log.d("arcirk Intent URI:", "is null");
            return;
      }

      Log.d("arcirk Intent URI:", intentUri.toString());

      // content or file
      intentScheme = intentUri.getScheme();
      if (intentScheme == null){
            Log.d("arcirk Intent URI Scheme:", "is null");
            return;
      }
      if(intentScheme.equals("file")){
            // URI as encoded string
            Log.d("arcirk Intent File URI: ", intentUri.toString());
            setFileUrlReceived(intentUri.toString());
            // we are done Qt can deal with file scheme
            return;
      }
      if(!intentScheme.equals("content")){
              Log.d("arcirk Intent URI unknown scheme: ", intentScheme);
              return;
      }
      // ok - it's a content scheme URI
      // we will try to resolve the Path to a File URI
      // if this won't work or if the File cannot be opened,
      // we'll try to copy the file into our App working dir via InputStream
      // hopefully in most cases PathResolver will give a path

      // you need the file extension, MimeType or Name from ContentResolver ?
      // here's HowTo get it:
      Log.d("arcirk Intent Content URI: ", intentUri.toString());
      ContentResolver cR = this.getContentResolver();
      MimeTypeMap mime = MimeTypeMap.getSingleton();
      String fileExtension = mime.getExtensionFromMimeType(cR.getType(intentUri));
      Log.d("arcirk","Intent extension: "+fileExtension);
      String mimeType = cR.getType(intentUri);
      Log.d("arcirk"," Intent MimeType: "+mimeType);
      String name = QShareUtils.getContentName(cR, intentUri);
      if(name != null) {
           Log.d("arcirk Intent Name:", name);
      } else {
           Log.d("arcirk Intent Name:", "is NULL");
      }
      String filePath;
      filePath = QSharePathResolver.getRealPathFromURI(this, intentUri);
      if(filePath == null) {
            Log.d("arcirk QSharePathResolver:", "filePath is NULL");
      } else {
            Log.d("arcirk QSharePathResolver:", filePath);
            // to be safe check if this File Url really can be opened by Qt
            // there were problems with MS office apps on Android 7
            if (checkFileExits(filePath)) {
                setFileUrlReceived(filePath);
                // we are done Qt can deal with file scheme
                return;
            }
      }

      // trying the InputStream way:
      filePath = QShareUtils.createFile(cR, intentUri, workingDirPath);
      if(filePath == null) {
           Log.d("arcirk Intent FilePath:", "is NULL");
           return;
      }
      setFileReceivedAndSaved(filePath);
    } // processIntent

} // class QShareActivity
