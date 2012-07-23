<?php
/**
 * @file
* Support for migrate module
*/

/**
 * Dependant migration that creates Party's from Profile2's.
 */
class PartyB2BRelationMigration extends Migration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Migration of organizations from Profile2 to Party entities.');
    $this->dependencies = array('PartyB2BOrganizationParty', 'PartyB2BOrganizationParty');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'org_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('party')
    );

    // MigrateSource
    // This should be the same CSV file as the MigrateSource for PartyOrganizationProfile2 migration.
    $this->source = new MigrateSourceCSV(__DIR__ . '/org.csv', $this->csvcolumns(),array('header_rows'=>1));

    // MigrateDestination
    $this->destination = new MigrateDestinationEntityAPI(
      'party',  // Entity type
      'party'   // bundle
    );

    // Field Mappings
    $this->addFieldMapping('profile2_id', 'org_id')
      ->sourceMigration('PartyB2BOrganizationProfile2');
    $this->addFieldMapping('label', 'org_name');
  }

  /**
   * Define cs columns.
   *
   * @return multitype:string multitype:string
   */
  function csvcolumns() {
    $columns[0] = array('org_id', 'Organization ID');
    $columns[] = array('org_name', 'Organization Name');
    return $columns;
  }

  /**
   * Attach profile2 from PartyOrganizationProfile2Migration
   */
  function complete($party, $row) {
    // Set the organization on these parties
    $wrapper = entity_metadata_wrapper('party', $party);
    $wrapper->party_hat->set('org');
    $wrapper->save();

    $profile2 = profile2_load($party->profile2_id);
    party_attach_entity($party, $profile2, 'profile2_party_org');
  }
}
