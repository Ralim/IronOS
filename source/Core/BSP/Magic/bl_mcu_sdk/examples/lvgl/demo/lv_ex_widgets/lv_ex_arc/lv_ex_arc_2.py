def arc_loader(task,arc):
    angle = arc.get_value()
    # print("angle: ",angle)
    angle += 5
    if angle > 360:
        task._del()
    else:
        arc.set_value(angle)

# create an arc which acts as a loader

arc = lv.arc(lv.scr_act(),None)
arc.set_range(0,360)
arc.set_bg_angles(0,360)
arc.set_angles(0,360)
arc.set_rotation(270)
arc.align(None,lv.ALIGN.CENTER,0,0)
arc.set_value(10)

# Create an `lv_task` to update the arc.

task = lv.task_create_basic()
task.set_cb(lambda task: arc_loader(task, arc))
task.set_period(20)
task.set_prio(lv.TASK_PRIO.LOWEST)
