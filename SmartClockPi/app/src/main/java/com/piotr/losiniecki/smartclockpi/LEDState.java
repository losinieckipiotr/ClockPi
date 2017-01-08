package com.piotr.losiniecki.smartclockpi;

import android.util.JsonReader;

import java.io.StringReader;

/**
 * Created by Anwen on 2017-01-08.
 */

public class LEDState {
    static public Integer getFromJSON(String json) {
        try {
            Integer state = null;
            JsonReader reader = new JsonReader(new StringReader(json));

            reader.beginObject();
            if (reader.nextName().equals("state"))
                state = Integer.parseInt(reader.nextString());
            return state;
        } catch (Exception e) {
            return null;
        }
    }
}
