<?php

/**
 * Filter by attached entity bundle.
 */
class party_handler_filter_party_attached_entity_bundle extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Attached entity types');
      $data_sets = party_get_data_set_info();

      // Get the data types from the entity type filter that should also be
      // on this display if it's been set up to make sense.
      // @todo: probably add validation, including checking the table of the filter.
      $entity_types = $this->view->display_handler->handlers['filter']['entity_type']->options['value'];

      //dsm($this->view);
      //dsm($this->view->display_handler->options['filters']);

      $options = array();
      $options = array();
      foreach ($data_sets as $data_set => $info) {
        if (in_array($info['entity type'], $entity_types)) {
          // @todo: show labels rather than machine names.
          $options[$info['entity bundle']] = $info['entity bundle'];
        }
      }

      asort($options);
      $this->value_options = $options;
    }
  }
}
