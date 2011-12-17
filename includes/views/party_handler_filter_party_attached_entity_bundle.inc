<?php

/**
 * Filter by attached entity bundle.
 */
class party_handler_filter_party_attached_entity_bundle extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      // @todo This is a quickie hack to make it work.
      // We come here at various points in a view's lifecycle and not all
      // of them have the earlier entity type filter which we rely on.
      // Hence when the earlier filter is not set, we should just return all
      // bundles which are valid in data sets.
      if (!count($this->view->display_handler->options['filters'])) {
        return array();
      }

      $this->value_title = t('Attached entity types');
      $data_sets = party_get_data_set_info();
      // Get the data types from the entity type filter that should also be
      // on this display if it's been set up to make sense.
      // @todo: probably add validation.
      $entity_types = $this->view->display_handler->options['filters']['entity_type']['value'];
      $options = array();
      foreach ($data_sets as $data_set => $info) {
        if (in_array($info['entity type'], $entity_types)) {
          $options[$info['entity bundle']] = $info['entity bundle'];
        }
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}
