package org.wijg.swood.pipower;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * Created by swood on 3/4/2017.
 */

public class PiSwitchAdapter extends BaseAdapter {

    Context context;
    List<PiSwitch> piSwitches;

    // temporary working pointer - should get from settings - should ideally call external class
    String url = "http://thewild.dyndns.org:44080/rest.php";

    PiSwitchAdapter(Context context, List<PiSwitch> piSwitches) {
        this.context = context;
        this.piSwitches = piSwitches;
    }

    @Override
    public int getCount() {
        return piSwitches.size();
    }

    @Override
    public Object getItem(int position) {
        return piSwitches.get(position);
    }

    @Override
    public long getItemId(int position) {
        return piSwitches.indexOf(getItem(position));
    }

    /* private view holder class */
    private class ViewHolder {
        Button piToggle1;
        Switch piStatus1;
        TextView piInfo1;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {

        ViewHolder holder = null;

        LayoutInflater mInflater = (LayoutInflater) context
                .getSystemService(Activity.LAYOUT_INFLATER_SERVICE);
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.listview_piswitch, null);
            holder = new ViewHolder();

            holder.piToggle1 = (Button) convertView
                    .findViewById(R.id.piToggle1);
            holder.piStatus1 = (Switch) convertView
                    .findViewById(R.id.piStatus1);
            holder.piInfo1 = (TextView) convertView.findViewById(R.id.piInfo1);

            PiSwitch row_pos = piSwitches.get(position);

            holder.piToggle1.setOnClickListener(
                    new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            PiSwitch o = (PiSwitch) piSwitches.get(position);
                            makeToggleRequest(v, o.getSwitchPin());
                        }
                    }
            );
            holder.piToggle1.setText(row_pos.getPiId());
            holder.piStatus1.setChecked(row_pos.getSwitchState());
            holder.piStatus1.setText(row_pos.getPiName());
            holder.piInfo1.setText(row_pos.getPiAddress());

            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        return convertView;
    }


    private void makeToggleRequest(final View view, Integer pin)
    {
        Toast.makeText(context, "Toggling: " + pin, Toast.LENGTH_SHORT).show();

        Map<String, String> params = new HashMap();
        params.put("func", "toggle");
        params.put("pin", pin.toString());

        JSONObject parameters = new JSONObject(params);

        JsonObjectRequest jsonRequest = new JsonObjectRequest(
                Request.Method.POST, url, parameters,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        Toast.makeText(context, "Response: " + response.toString(), Toast.LENGTH_SHORT).show();
                        try {
                            Switch togSw = (Switch) view.findViewById(R.id.piStatus1);
                            togSw.setChecked((response.getInt("state") == 1) ? true : false);
                        } catch(JSONException e) {
                            //DisplayToast("'state' type mismatch");
                        }
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        //error.printStackTrace();
                        Toast.makeText(context, "Error: "+error.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                }
        );

        Volley.newRequestQueue(context).add(jsonRequest);
    }

}
