package com.piotr.losiniecki.smartclockpi;

/**
 * Created by Anwen on 2016-12-18.
 */

import android.text.format.Time;

import java.io.StringReader;
import android.util.JsonReader;

public class Result {
    public double temperature = Double.NaN;
    public double pressure = Double.NaN;
    Time timeStamp = new Time();

    static public Result getFromJSON(String json) {
        try {
            Result res = new Result();
            JsonReader reader = new JsonReader(new StringReader(json));
            reader.beginObject();
            if (reader.nextName().equals("result")) {
                reader.beginObject();
                if (reader.nextName().equals("temperature")) {
                    res.temperature = reader.nextDouble();
                }
                if (reader.nextName().equals("pressure")) {
                    res.pressure = reader.nextDouble();
                }
                if (reader.nextName().equals("timeStamp")) {
                    res.timeStamp.set(reader.nextLong());
                }
            } else {
                res = null;
            }
            return res;
        } catch (Exception e) {
            return null;
        }
    }
}
