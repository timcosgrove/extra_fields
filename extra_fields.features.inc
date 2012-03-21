<?php

function extra_fields_features_export($data, &$export, $module_name = '') {
  $pipe = array(); // formality as we're currently not adding any dependencies
  $map = features_get_default_map('extra_fields');
  foreach ($data as $identifier) {
    list($entity_type, $bundle) = explode(':', $identifier);
    if ($settings = extra_fields_extra_fields_features_load($entity_type, $bundle)) {
      if (isset($map[$identifier]) && $map[$identifier] != $module_name) {
        if (isset($export['features']['extra_fields'][$identifier])) {
          unset($export['features']['extra_fields'][$identifier]);
        }
        $module = $map[$identifier];
        $export['dependencies'][$module] = $module;
      }
      else {
        $export['features']['extra_fields'][$identifier] = $identifier;
      }
    }
  }
  return $pipe;
}

function extra_fields_features_export_options() {
  $options = array();
  foreach (extra_fields_extra_fields_features_load() as $entity_type => $bundles) {
    foreach ($bundles as $bundle_name => $bundle_info) {
      $options["${entity_type}:${bundle_name}"] = "${entity_type}: ${bundle_name} extra fields";
    }
  }
  return $options;
}

function extra_fields_features_export_render($module_name, $data, $export = NULL) {
  error_log(print_r($data, TRUE));
  $code = array();
  $code[] = ' $extra_fields_info = array();';
  foreach ($data as $name) {
    list($entity_type, $bundle) = explode(':', $name);
    $code[] = "  \$extra_fields_info['{$entity_type}']['{$bundle}']['extra_fields'] = " . features_var_export(extra_fields_extra_fields_features_load($entity_type, $bundle)) .";";
  }
  $code[] = "return \$extra_fields_info;";
  $code = implode("\n", $code);
  return array('extra_fields_info' => $code);
}

function extra_fields_features_revert($module) {
  if ($default_types = features_get_default('extra_fields', $module)) {
    foreach ($default_types as $entity_type => $entity_info) {
      foreach ($entity_info as $bundle_name => $bundle_info) {
        $field_bundle_settings = field_bundle_settings($entity_type, $bundle_name);
        $field_bundle_settings['extra_fields'] = $bundle_info['extra_fields'];
        field_bundle_settings($entity_type, $bundle_name, $field_bundle_settings);
      }
    }
  }
}

function extra_fields_extra_fields_features_load($entity_type = NULL, $bundle = NULL) {
  $extra_fields = array();
  if ($field_bundle_settings = variable_get('field_bundle_settings')) {
    foreach ($field_bundle_settings as $_entity_type => $bundle_info) {
      foreach ($bundle_info as $_bundle => $bundle_settings) {
        if (!empty($bundle_settings['extra_fields'])) {
          $extra_fields[$_entity_type][$_bundle]['extra_fields'] = $bundle_settings['extra_fields'];
        }
      }
    }
    // pull out the items that are already available in default
  /*  $map = features_get_default_map('extra_fields');
    foreach ($map as $module) {
      $module_defaults = features_get_default('extra_fields', $module);
      foreach ($module_defaults as $_entity_type => $entity_info) {
        foreach ($entity_info as $_bundle_name => $bundle_info) {
          // if these are default, we don't want to list them in the options
          // or allow them to be exported
          unset($extra_fields[$_entity_type][$_bundle_name]);
        }
      }
    }*/
    if (!empty($entity_type) && !empty($bundle)) {
      return $extra_fields[$entity_type][$bundle]['extra_fields'];
    }
  }
  return $extra_fields;
}