package com.piotr.losiniecki.smartclockpi;

/**
 * Created by Anwen on 2016-12-18.
 */

import java.io.StringReader;
import android.util.JsonReader;

public class Response {

    public int id = -1;
    public int state = 0;
    public int lenght = 0;

    static public Response getFromJSON(String json) {
        try {
            Response resp = new Response();
            JsonReader reader = new JsonReader(new StringReader(json));
            reader.beginObject();
            if (reader.nextName().equals("response")) {
                reader.beginObject();
                if (reader.nextName().equals("id"))
                    resp.id = reader.nextInt();
                if (reader.nextName().equals("state"))
                    resp.state = reader.nextInt();
                if (reader.nextName().equals("length"))
                    resp.lenght = reader.nextInt();
                reader.endObject();
                reader.endObject();
            } else {
                resp = null;
            }
            return resp;
        } catch (Exception e) {
            return null;
        }
    }
}
