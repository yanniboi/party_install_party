<?php
/**
 * @file party_subscription_settings.entity.inc
 *   Classes and Controllers for the Party Subscription Settings entity
 */

/**
 * The class used for Application Type entities
 */
class PartySubscriptionSettings extends Entity {
  public function __construct($values = array()) {
    $values += array(
      'data' => array(),
    );
    parent::__construct($values, 'party_subscription_settings');
  }
  
  public function save() {
    parent::save();
  }
}

/**
 * The Controller for Application Type entities
 */
class PartySubscriptionSettingsController extends EntityAPIController {
  public function __construct($entityType = 'party_subscription_settings') {
    parent::__construct("party_subscription_settings");
  }
}
