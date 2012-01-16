<?php
/**
 * @file
 * Hooks provided by the Party Hats module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Act on a Party when some Hats are assigned.
 *
 * @param $party
 *   The party the hats have been assigned to
 * @param $hats
 *   The hats that have been assigned to the party. An array of hat objects
 *   keyed by hat name
 *
 * @see party_hat_assign_hat()
 */
function hook_party_hat_assign_hats($party, $hats) {
}

/**
 * Act on a Party when some Hats are unassigned.
 *
 * @param $party
 *   The party the hats have been unassigned from
 * @param $hats
 *   The hats that have been unassigned from the party. An array of hat objects
 *   keyed by hat name
 *
 * @see party_hat_unassign_hats()
 */
function hook_party_hat_unassign_hats($party, $hats) {
}

/**
 * @}
 */

/**
 * Assign hats to a party
 *
 * @param $party
 *   The party to assign hats to
 * @param $hats
 *   The hats to be assigned. An array of hat objects keyed by name
 */
function party_hat_assign_hats($party, $hats) {
  // Get the hat items from the party object
  $hat_items = field_get_items('party', $party, 'party_hat');
  
  foreach ($hats as $hat) {
    $has_hat = FALSE;
    foreach ($hat_items as $item) {
      if ($item['hat_name'] == $hat->hat_name) {
        $has_hat = TRUE;
      }
    }
    
    // Don't add the hat if the Party already has it
    if ($has_hat) {
      continue;
    }
    
    $assigned_hats[$hat->name] = $hat;
    $hat_items[]['hat_name'] = $hat->name;   
  }
  
  $party->party_hat[LANGUAGE_NONE] = $hat_items;
  party_save($party);
  
  if (module_exists('rules') {
    rules_invoke_all('party_hat_assign_hats', $party, $assigned_hats);
  }
  else {
    module_invoke_all('party_hat_assign_hats', $party, $assigned_hats);
  }
}

/**
 * Unassign hats
 *
 * @param $party
 * @param $hats
 *   An array of hats to unassign.
 */
function party_hats_unassign_hats($party, $hats) {
  // Get the hat items from the party object
  $hat_items = field_get_items('party', $party, 'party_hat');
  
  foreach ($hats as $hat) {
    $has_hat = FALSE;
    foreach ($hat_items as $delta => $item) {
      if ($item['hat_name'] == $hat->hat_name) {
        $has_hat = TRUE;
        unset($hat_items[$delta]);
      }
    }
    
    // Don't remove the hat if its not assigned
    if (!$has_hat) {
      continue;
    }    
    $unassigned_hats[$hat->name] = $hat;
       
  }
  
  $party->party_hat[LANGUAGE_NONE] = $hat_items;
  party_save($party);
  
  if (module_exists('rules') {
    rules_invoke_all('party_hat_unassign_hats', $party, $unassigned_hats);
  }
  else {
    module_invoke_all('party_hat_unassign_hats', $party, $unassigned_hats);
  }
}
 