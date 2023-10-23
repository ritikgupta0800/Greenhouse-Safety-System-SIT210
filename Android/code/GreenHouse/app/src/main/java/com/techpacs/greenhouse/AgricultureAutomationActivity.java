package com.techpacs.greenhouse;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;

import com.google.android.material.button.MaterialButtonToggleGroup;
import com.techpacs.greenhouse.models.DataModifiedModel;
import com.techpacs.greenhouse.models.ResponseModel;
import com.techpacs.greenhouse.retorfitClientAndService.RetrofitClient;
import com.techpacs.greenhouse.retorfitClientAndService.WebServices;

import java.util.Objects;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class AgricultureAutomationActivity extends AppCompatActivity {
    private final int ID = 2;
    private final Context context = AgricultureAutomationActivity.this;
    TextView tvTemperature;
    TextView tvMoisture;
    TextView tvHumidity;
    TextView tvThresholdTemperature;
    TextView tvThresholdMoisture;
    SeekBar seekBarThresholdTemperature;
    SeekBar seekBarThresholdMoisture;
    SwitchCompat swLight, swFan, swWaterPump;
    MaterialButtonToggleGroup mbtg_mode;
    LinearLayout ll_manual, ll_automatic;
    TextView tv_last_update_time;
    private String last_update_time;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_agriculture_automation);
        init();
        listeners();
        getDataFromServer(true);
    }

    void init() {


        swLight = findViewById(R.id.swLight);
        swFan = findViewById(R.id.swFan);
        swWaterPump = findViewById(R.id.swWaterPump);
        seekBarThresholdMoisture = findViewById(R.id.seekBarThresholdMoisture);
        seekBarThresholdTemperature = findViewById(R.id.seekBarThresholdTemperature);
        tvThresholdTemperature = findViewById(R.id.tvThresholdTemperature);
        tvThresholdMoisture = findViewById(R.id.tvThresholdMoisture);
        tvTemperature = findViewById(R.id.tvTemperature);
        tvHumidity = findViewById(R.id.tvHumidity);
        tvMoisture = findViewById(R.id.tvMoisture);
        mbtg_mode = findViewById(R.id.mbtg_mode);
        tv_last_update_time = findViewById(R.id.tv_last_update_time);
        ll_manual = findViewById(R.id.ll_manual);
        ll_automatic = findViewById(R.id.ll_automatic);
        mbtg_mode.uncheck(R.id.btn_manual);
        mbtg_mode.check(R.id.btn_automatic);
        ll_manual.setVisibility(View.GONE);
        ll_automatic.setVisibility(View.VISIBLE);

        last_update_time = getString(R.string.last_update_time);
    }



    private void listeners() {

        mbtg_mode.addOnButtonCheckedListener(new MaterialButtonToggleGroup.OnButtonCheckedListener() {
            @Override
            public void onButtonChecked(MaterialButtonToggleGroup group, int checkedId, boolean isChecked) {
                if (isChecked) {
                    if (checkedId==R.id.btn_automatic) {

                            mbtg_mode.uncheck(R.id.btn_manual);
                            ll_manual.setVisibility(View.GONE);
                            ll_automatic.setVisibility(View.VISIBLE);
                            sendDataToServer(9, "A");
                    }
                        if (checkedId==R.id.btn_manual) {

                            ll_manual.setVisibility(View.VISIBLE);
                            ll_automatic.setVisibility(View.GONE);
                            mbtg_mode.uncheck(R.id.btn_automatic);
                            sendDataToServer(9, "M");
                        }
                    }
                }

        });
        seekBarThresholdTemperature.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                int i  = seekBar.getProgress();
                tvThresholdTemperature.setText(i + " °C");
                sendDataToServer(2, String.valueOf(i));
            }
        });
        seekBarThresholdMoisture.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                int i  = seekBar.getProgress();

                tvThresholdMoisture.setText(i + " %");
                sendDataToServer(4, String.valueOf(i));
            }
        });

        tvTemperature.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                getDataFromServer(1);
            }
        });
        tvMoisture.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                getDataFromServer(3);
            }
        });
        tvHumidity.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                getDataFromServer(5);
            }
        });


        swFan.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                sendDataToServer(8, b ? "1" : "0");
            }
        });
        swWaterPump.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                sendDataToServer(7, b ? "1" : "0");
            }
        });
        swLight.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                sendDataToServer(6, b ? "1" : "0");
            }
        });
    }


    private void sendDataToServer(int field, String data) {
        WebServices webServices = RetrofitClient.getClient(Constants.SERVER_NAME).create(WebServices.class);


        webServices.set_fields(ID, field, data).enqueue(new Callback<ResponseModel>() {
            @Override
            public void onResponse(Call<ResponseModel> call, Response<ResponseModel> response) {
//                if (response.body() != null)
//                    Toast.makeText(context, "Data uploaded", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Call<ResponseModel> call, Throwable t) {

            }
        });
    }

    private void getDataFromServer(int field) {
        WebServices webServices = RetrofitClient.getClient(Constants.SERVER_NAME).create(WebServices.class);
        webServices.get_field(ID, field).enqueue(new Callback<Integer>() {
            @Override
            public void onResponse(Call<Integer> call, Response<Integer> response) {
                if (response.body() != null)
                    set_data(field, response.body());

            }

            @Override
            public void onFailure(Call<Integer> call, Throwable t) {

            }
        });
    }


    private void set_data(int field, int value) {
        switch (field) {
            case 1:
                tvTemperature.setText(value + " °C");
                break;
            case 3:
                tvMoisture.setText(value + " %");
                break;
            case 5:
                tvHumidity.setText(value + " %");
                break;
        }
    }

    private void getDataFromServer(boolean b) {
        WebServices webServices = RetrofitClient.getClient(Constants.SERVER_NAME).create(WebServices.class);
        webServices.get_fields(ID, "android").enqueue(new Callback<DataModifiedModel>() {
            @Override
            public void onResponse(Call<DataModifiedModel> call, Response<DataModifiedModel> response) {
                DataModifiedModel dataModel = response.body();
                if (dataModel != null) {
                    set_data(1, Integer.parseInt(dataModel.getField1().getValue()));
                    set_data(3, Integer.parseInt(dataModel.getField3().getValue()));
                    set_data(5, Integer.parseInt(dataModel.getField5().getValue()));
                    tv_last_update_time.setText(String.format(last_update_time, dataModel.getField1().getLast_update_time()));
                    if (b) {

                        seekBarThresholdTemperature.setProgress(Integer.parseInt(dataModel.getField2().getValue()));
                        tvThresholdTemperature.setText(dataModel.getField2().getValue() + " °C");

                        seekBarThresholdMoisture.setProgress(Integer.parseInt(dataModel.getField4().getValue()));
                        tvThresholdMoisture.setText(dataModel.getField4().getValue() + " %");
                        if (Objects.equals(dataModel.getField9().getValue(), "M")) {
                            mbtg_mode.uncheck(R.id.btn_automatic);
                            mbtg_mode.check(R.id.btn_manual);
                            ll_manual.setVisibility(View.VISIBLE);
                            ll_automatic.setVisibility(View.GONE);
                        } else {
                            mbtg_mode.uncheck(R.id.btn_manual);
                            mbtg_mode.check(R.id.btn_automatic);
                            ll_manual.setVisibility(View.GONE);
                            ll_automatic.setVisibility(View.VISIBLE);
                        }
                        swLight.setChecked(Objects.equals(dataModel.getField6().getValue(), "1"));
                        swWaterPump.setChecked(Objects.equals(dataModel.getField7().getValue(), "1"));
                        swFan.setChecked(Objects.equals(dataModel.getField8().getValue(), "1"));
                    }
                }
            }

            @Override
            public void onFailure(Call<DataModifiedModel> call, Throwable t) {

            }
        });
    }


    Handler handler = new Handler();
    Runnable runnable;
    int delay = 5000;

    @Override
    protected void onResume() {
        handler.postDelayed(runnable = new Runnable() {
            public void run() {
                handler.postDelayed(runnable, delay);
                getDataFromServer(false);
            }
        }, delay);
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        handler.removeCallbacks(runnable); //stop handler when activity not visible super.onPause();
    }

}