<?php
// $Id$

/**
 * Implements hook_drush_help().
 */
function cnapi_drush_help($section) {
  switch ($section) {
    case 'cnapi:cnapi-import-values':
      return dt("Import all Cultuurnet API headings, locations, ...");
  }
}


/**
 * Implements hook_drush_command().
 */
function cnapi_drush_command() {
  $items['cnapi-import-values'] = array(
    'description' => 'Import all Cultuurnet API values.',
    'examples' => array(
      'drush cnapi-import-values' => '(Re-)import all Cultuurnet API headings, cities, regions, output types, ...',
    ),
  );

  return $items;
}

function drush_cnapi_import_values() {
  require_once 'cnapi.import.inc';
  cnapi_import();
}