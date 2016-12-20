package com.piotr.losiniecki.smartclockpi;

import android.util.JsonReader;

import java.io.StringReader;

/**
 * Created by Anwen on 2016-12-19.
 */

public class AlarmTime {
    public int hour = -1;
    public  int minute = -1;
    public AlarmTime(int hour, int minute) {
        this.hour = hour;
        this.minute = minute;
    }

    static public AlarmTime getFromJSON(String json) {
        try {
            AlarmTime retAlarm = new AlarmTime(-1, -1);
            JsonReader reader = new JsonReader(new StringReader(json));

            reader.beginObject();
            if (reader.nextName().equals("hour"))
                retAlarm.hour = Integer.parseInt(reader.nextString());
            if (reader.nextName().equals("minute"))
                retAlarm.minute = Integer.parseInt(reader.nextString());
            return (retAlarm.hour != -1 && retAlarm.minute != -1) ? retAlarm : null;
        } catch (Exception e) {
            return null;
        }
    }

    @Override
    public String toString() {
        if( hour != 0 && minute != 0)
            return Integer.toString(hour) + ":" + Integer.toString(minute);
        else
            return new String("Alarm not set");
    }
}
