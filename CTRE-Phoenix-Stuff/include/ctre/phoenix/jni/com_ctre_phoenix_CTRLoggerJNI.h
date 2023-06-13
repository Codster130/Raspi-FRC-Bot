/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_ctre_phoenix_CTRLoggerJNI */

#ifndef _Included_com_ctre_phoenix_CTRLoggerJNI
#define _Included_com_ctre_phoenix_CTRLoggerJNI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_ctre_phoenix_CTRLoggerJNI
 * Method:    JNI_Logger_Close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ctre_phoenix_CTRLoggerJNI_JNI_1Logger_1Close
  (JNIEnv *, jclass);

/*
 * Class:     com_ctre_phoenix_CTRLoggerJNI
 * Method:    JNI_Logger_Log
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_ctre_phoenix_CTRLoggerJNI_JNI_1Logger_1Log
  (JNIEnv *, jclass, jint, jstring, jstring);

/*
 * Class:     com_ctre_phoenix_CTRLoggerJNI
 * Method:    JNI_Logger_Open
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ctre_phoenix_CTRLoggerJNI_JNI_1Logger_1Open
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_ctre_phoenix_CTRLoggerJNI
 * Method:    JNI_Logger_GetShort
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ctre_phoenix_CTRLoggerJNI_JNI_1Logger_1GetShort
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_ctre_phoenix_CTRLoggerJNI
 * Method:    JNI_Logger_GetLong
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ctre_phoenix_CTRLoggerJNI_JNI_1Logger_1GetLong
  (JNIEnv *, jclass, jint);

#ifdef __cplusplus
}
#endif
#endif