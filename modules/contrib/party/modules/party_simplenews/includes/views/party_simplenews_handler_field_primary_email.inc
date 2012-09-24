<?php
/**
 * @file
 * Contains a Views field handler to take care of displaying deletes links
 * as fields
 */

class party_simplenews_handler_field_primary_email extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['pid'] = 'pid';
    $this->additional_fields['type'] = 'type';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $type = $values->{$this->aliases['type']};
    $party_id = $values->{$this->aliases['pid']};

    //Creating a dummy party to check access against
    $dummy_party = (object) array('pid' => $party_id, 'type' => $type);
    
    return reset(party_get_all_emails($dummy_party));
  }
}
