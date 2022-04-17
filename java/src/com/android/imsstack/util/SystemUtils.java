
package com.android.imsstack.util;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

/**
 * This class provides the wrapper APIs for Android Platform.
 */
public final class SystemUtils {
    public static final String UTC_TIME_FORMAT = "yyyy-MM-dd'T'HH:mm:ss";

    public static String calculateMessageDigest(String algorithm, String input) {
        MessageDigest md = null;

        try {
            md = MessageDigest.getInstance(algorithm);
            md.update(input.getBytes());
        } catch (NoSuchAlgorithmException e) {
            ImsLog.e(e.toString());
            e.printStackTrace();
            md = null;
        } catch (Exception e) {
            ImsLog.e(e.toString());
            e.printStackTrace();
            md = null;
        }

        if (md == null) {
            return null;
        }

        return toHexString(md.digest());
    }

    public static String getUTCTimeFormat(long millis) {
        SimpleDateFormat sdf = new SimpleDateFormat(UTC_TIME_FORMAT, Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        return sdf.format(new Date(millis)) + "Z";
    }

    public static String toHexString(byte[] input) {
        if (input == null) {
            return null;
        }

        String output = "";
        String hex;

        for (int i = 0; i < input.length; ++i) {
            hex = Integer.toHexString(input[i] & 0xff);

            if (hex.length() == 1) {
                hex = "0".concat(hex);
            }

            output += hex.toLowerCase();
        }

        return output;
    }
}
