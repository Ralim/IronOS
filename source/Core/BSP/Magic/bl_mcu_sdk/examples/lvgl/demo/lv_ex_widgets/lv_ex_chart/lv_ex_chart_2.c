#include "../../../lv_examples.h"
#if LV_USE_CHART

/**
 * Add a faded area effect to the line chart
 */
void lv_ex_chart_2(void)
{
    /*Create a chart*/
    lv_obj_t *chart;
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE); /*Show lines and points too*/

    /*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255); /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);   /*Transparent on the bottom*/

    /*Add two data series*/
    lv_chart_series_t *ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t *ser2 = lv_chart_add_series(chart, LV_COLOR_GREEN);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, ser1, 31);
    lv_chart_set_next(chart, ser1, 66);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 89);
    lv_chart_set_next(chart, ser1, 63);
    lv_chart_set_next(chart, ser1, 56);
    lv_chart_set_next(chart, ser1, 32);
    lv_chart_set_next(chart, ser1, 35);
    lv_chart_set_next(chart, ser1, 57);
    lv_chart_set_next(chart, ser1, 85);

    /*Directly set points on 'ser2'*/
    ser2->points[0] = 92;
    ser2->points[1] = 71;
    ser2->points[2] = 61;
    ser2->points[3] = 15;
    ser2->points[4] = 21;
    ser2->points[5] = 35;
    ser2->points[6] = 35;
    ser2->points[7] = 58;
    ser2->points[8] = 31;
    ser2->points[9] = 53;

    lv_chart_refresh(chart); /*Required after direct set*/
}

#endif
