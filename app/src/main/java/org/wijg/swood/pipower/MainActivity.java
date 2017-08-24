package org.wijg.swood.pipower;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity implements OnItemClickListener {

    String url = "http://thewild.dyndns.org:44080/rest.php";

    List<PiSwitch> piSwitches;
    ListView piSwitchListview;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        piSwitches = new ArrayList<PiSwitch>();

        PiSwitch switch1 = new PiSwitch();
        switch1.setPiId("p1");
        switch1.setPiName("RaspPi1");
        switch1.setSwitchPin(1);
        switch1.setSwitchState(0);
        switch1.setPiAddress("192.168.1.101");

        piSwitches.add(switch1);

        PiSwitch switch2 = new PiSwitch();
        switch2.setPiId("p2");
        switch2.setPiName("RaspPi2");
        switch2.setSwitchPin(2);
        switch2.setSwitchState(false);
        switch2.setPiAddress("192.168.1.102");

        piSwitches.add(switch2);

        piSwitchListview = (ListView) findViewById(R.id.switch_list);
        PiSwitchAdapter adapter = new PiSwitchAdapter(this, piSwitches);
        piSwitchListview.setAdapter(adapter);

        piSwitchListview.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        String member_name = piSwitches.get(position).getPiName();
        Toast.makeText(getApplicationContext(), "" + member_name, Toast.LENGTH_SHORT).show();
    }

    /**
     * display a text message temporarily near the bottom of the display
     *
     * @param msg
     */
    private void DisplayToast(String msg) {
        Toast.makeText(getBaseContext(), msg, Toast.LENGTH_SHORT).show();
    }

    /**
     * Display a lasting text message below the dynamic fragments
     *
     * @param msg
     */
    private void DisplayText(String msg)
    {
        TextView textField;
        textField = (TextView) findViewById(R.id.ajaxStatus);
        textField.setText(msg);
    }
}



