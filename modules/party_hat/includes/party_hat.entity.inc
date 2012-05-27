<?php
/**
 * @file
 * Contains the controller classes for Party entities.
 */

/**
 * The API controller class for the Party Hat entity.
 */
class PartyHatController extends EntityAPIController {

}

/**
 * The class used for hat entities.
 */
class PartyHat extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'party_hat');
  }

  protected function defaultLabel() {
    return $this->label;
  }

  protected function defaultUri() {
    return array('path' => 'party-hat/' . $this->id);
  }
}

/**
 * UI controller.
 */
class PartyHatUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = 'Manage hats';
    return $items;
  }
}
