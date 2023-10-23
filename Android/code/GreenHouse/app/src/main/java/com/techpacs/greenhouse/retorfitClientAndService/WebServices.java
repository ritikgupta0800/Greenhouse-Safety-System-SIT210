package com.techpacs.greenhouse.retorfitClientAndService;


import com.techpacs.greenhouse.models.DataModifiedModel;
import com.techpacs.greenhouse.models.ResponseModel;

import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Query;

public interface WebServices {

    @GET("/bgiet/iot/get_values.php")
    Call<Integer> get_field(@Query("id") int id, @Query("field") int field);

    @GET("/bgiet/iot/get_values.php")
    Call<DataModifiedModel> get_fields(@Query("id") int id, @Query("os") String os);


    @GET("/bgiet/iot/set.php")
    Call<ResponseModel> set_fields(@Query("id") int id,
                                   @Query("field") int field,
                                   @Query("value") String value);
}