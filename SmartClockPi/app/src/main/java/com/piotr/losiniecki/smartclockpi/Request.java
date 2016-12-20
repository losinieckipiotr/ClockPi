package com.piotr.losiniecki.smartclockpi;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by Anwen on 2016-12-19.
 */

import android.util.JsonWriter;
import java.io.StringWriter;

import android.util.Log;

public class Request {
    public int id;
    public String name;
    public Map<String, String> params;

    public Request(int id, String name) {
        this.id = id;
        this.name = name;
        params = new HashMap<>();
    }

    public String toJSON() {
        try {
            StringWriter str = new StringWriter();
            JsonWriter writer = new JsonWriter(str);

            writer.beginObject();
            writer.name("request");
            writer.beginObject();

            writer.name("id").value(id);
            writer.name("name").value(name);

            if (params.size() > 0) {
                writer.name("params");
                writer.beginObject();
                for (Map.Entry<String, String> entry : params.entrySet()) {
                    String key = entry.getKey();
                    String value = entry.getValue();
                    writer.name(key).value(value);
                }
                writer.endObject();
            } else {
                writer.name("params").value("");
            }

            writer.endObject();
            writer.endObject();

            writer.close();
            return str.toString();
        } catch (Exception e) {
            Log.e("Request", "toJSON", e);
            return null;
        }
    }

}
