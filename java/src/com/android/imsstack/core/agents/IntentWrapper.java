/**
 * IntentWrapper
 *         Role
 */

package com.android.imsstack.core.agents;

import android.content.Context;
import android.content.Intent;

import com.android.imsstack.util.ImsLog;

public class IntentWrapper {

    // Constants--------------------------------------------------
    // Variables--------------------------------------------------
    // Static loading materials ----------------------------------
    // Public methods --------------------------------------------
    public static void sendIntent(Context context, String action, boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + ")");

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }

    public static void sendIntent(Context context, String action, String name, int value,
            boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + "), name=" + name + ", value=" + value);

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        intent.putExtra(name, value);

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }

    public static void sendIntent(Context context, String action, String name1, int value1,
            String name2, int value2, boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + "), name1=" + name1 + ", value1=" + value1
            + ", name2=" + name2 + ", value2=" + value2);

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        intent.putExtra(name1, value1);
        intent.putExtra(name2, value2);

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }

    public static void sendIntent(Context context, String action, String name1, int value1,
            String name2, String value2, boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + "), name1=" + name1 + ", value1=" + value1
            + ", name2=" + name2 + ", value2=" + value2);

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        intent.putExtra(name1, value1);
        intent.putExtra(name2, value2);

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }

    public static void sendIntent(Context context, String action, String name1, String value1,
            String name2, String value2, String name3, String value3, boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + "), name1=" + name1 + ", value1=" + value1
            + ", name2=" + name2 + ", value2=" + value2
            + ", name3=" + name3 + ", value3=" + value3);

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        intent.putExtra(name1, value1);
        intent.putExtra(name2, value2);
        intent.putExtra(name3, value3);

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }

    public static void sendIntent(Context context, String action, String name, String value,
            boolean addBackgroudFlag) {
        ImsLog.i("sendIntent (" + action + "), name=" + name + ", value=" + value);

        Intent intent = new Intent(action);

        //4 Remove the flag if the intent is not required this flag
        intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
        if (addBackgroudFlag) {
            intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        }

        intent.putExtra(name, value);

        if (context == null) {
            return;
        }

        context.sendBroadcast(intent);
    }
    // Interface implementation methods --------------------------
    // Private/Protected methods ---------------------------------
}
