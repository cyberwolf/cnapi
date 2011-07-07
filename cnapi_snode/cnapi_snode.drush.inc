<?php

/**
 * @file
 * Drush integration for the CultuurNet API shadow nodes module
 */

/**
 * Implements hook_drush_help().
 */
function cnapi_snode_drush_help($section) {
  switch ($section) {
    case 'cnapi:cnapi-snode-cleanup':
      return dt("Clean up shadow nodes of objects that were removed on the API.");
  }
}


/**
 * Implements hook_drush_command().
 */
function cnapi_snode_drush_command() {
  $items['cnapi-snode-cleanup'] = array(
    'description' => 'Clean up shadow nodes.',
    'examples' => array(
      'drush cnapi-snode-cleanup' => 'Clean up shadow nodes of objects that were removed on the API',
    ),
  );

  return $items;
}

/**
 * Implements the 'cnapi-import-values' drush command.
 */
function drush_cnapi_snode_cleanup() {
  require_once 'cnapi_snode.helpers.inc';
  cnapi_snode_cleanup();
}