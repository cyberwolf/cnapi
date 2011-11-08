<?php

/**
 * @file
 * Drush integration for the CultuurNet API module.
 */

/**
 * Implements hook_drush_help().
 */
function cnapi_drush_help($section) {
  switch ($section) {
    case 'cnapi:cnapi-import-values':
      return dt("Import all CultuurNet API headings, locations, ...");
    case 'cnapi:cnapi-flush-caches':
      return dt("Flush all CultuurNet API caches.");
  }
}

/**
 * Implements hook_drush_command().
 */
function cnapi_drush_command() {
  $items['cnapi-import-values'] = array(
    'description' => 'Import all CultuurNet API values.',
    'examples' => array(
      'drush cnapi-import-values' => '(Re-)import all CultuurNet API headings, cities, regions, output types, ...',
    ),
  );
  $items['cnapi-flush-caches'] = array(
    'description' => 'Flush all CultuurNet API caches.',
    'examples' => array(
      'drush cnapi-flush-caches' => 'Flush all CultuurNet API caches.',
    ),
  );

  return $items;
}

/**
 * Implements the 'cnapi-import-values' drush command.
 */
function drush_cnapi_import_values() {
  require_once 'cnapi.import.inc';
  cnapi_import();
}

/**
 * Implements the 'cnapi-flush-caches' drush command.
 */
function drush_cnapi_flush_caches() {
  cache_clear_all('*', 'cache_cnapi', TRUE);
  cache_clear_all('*', 'cache_cnapi_detail', TRUE);
}