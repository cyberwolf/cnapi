<?php

/**
 * Row plugin for CNAPI Events
 */
class cnapi_views_row_production extends views_plugin_row {

  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);
    $this->base_table = $view->base_table;
    $this->base_field = 'cnapi_field';
  }

  /**
   * Render each $row.
   */
  function render($row) {
    return theme('cnapi_ui_production_summary', array('production' => $row));
  }
}
