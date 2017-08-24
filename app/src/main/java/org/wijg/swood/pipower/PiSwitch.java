package org.wijg.swood.pipower;

import android.os.Bundle;

/**
 * Created by swood on 3/4/2017.
 */

public class PiSwitch {
    private String piName = "";
    private String piId = "";
    private int switchPin = 0;
    private boolean switchState = false;
    private String piAddress = "";

    public void PiSwitch(String name) {
        this.setPiName(name);
    }

    public void PiSwitch(String name, String id, Integer pin) {
        this.setPiName(name);
        this.setPiId(id);
        this.setSwitchPin(pin);
    }

    public void PiSwitch(String name, String id, Integer pin, String addr) {
        this.setPiName(name);
        this.setPiId(id);
        this.setSwitchPin(pin);
        this.setPiAddress(addr);
    }

    public String getPiAddress() {
        return this.piAddress;
    }
    public String setPiAddress(String addr) {
        this.piAddress = addr;
        return this.getPiAddress();
    }

    public String getPiName() {
        return this.piName;
    }
    public String setPiName(String name) {
        this.piName = name;
        return this.getPiName();
    }

    public String getPiId() {
        return this.piId;
    }
    public String setPiId(String id) {
        this.piId = id.substring(0,2);
        return this.getPiId();
    }

    public int getSwitchPin() {
        return this.switchPin;
    }
    public int setSwitchPin(int pin) {
        this.switchPin = pin;
        return this.getSwitchPin();
    }

    public boolean getSwitchState() {
        return this.switchState;
    }
    public boolean setSwitchState(int state) {
        this.switchState = (state == 1) ? true : false;
        return this.getSwitchState();
    }
    public boolean setSwitchState(boolean state) {
        this.switchState = state;
        return this.getSwitchState();
    }
    public boolean toggleSwitchState() {
        this.switchState = !(this.switchState);
        return this.getSwitchState();
    }

    public Bundle getAsBundle() {
        Bundle bndl = new Bundle();
        bndl.putString("id", this.getPiId());
        bndl.putInt("pin", this.getSwitchPin());
        bndl.putString("name", this.getPiName());
        bndl.putBoolean("state", this.getSwitchState());
        bndl.putString("address", this.getPiAddress());
        return bndl;
    }
}
