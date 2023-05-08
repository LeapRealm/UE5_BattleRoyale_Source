import unreal

selected_items = unreal.EditorUtilityLibrary.get_selected_assets()
print("Num of items selected = " + str(len(selected_items)))

for item in selected_items:    
    print("Name=" + item.get_name() + " Class=" + item.get_class().get_name())

    sub_system = unreal.StaticMeshEditorSubsystem.get_default_object()
    lod_setting = sub_system.get_lod_build_settings(item, 0)
    lod_setting.distance_field_resolution_scale = 0.0
    sub_system.set_lod_build_settings(item, 0, lod_setting)
