Overview
--------

The Party module provides an extremely flexible CRM solution for Drupal,
utilizing a generic entity (the Party entity) to which other entities
(Drupal Users, Profile2 profiles, eCommerce customers, Facebook profiles, etc.)
are attached.

A Party entity does not have bundles and normally has no fields.  It is simply
a wrapper around other entities representing whatever, e.g. a Drupal User,
CiviCRM Constituant, Facebook account, eCommerce Customer, Employee profile,
etc. A Party may wrap one or more such entities.


Requirements
------------

PHP 5.3
Drupal greater than 7.14
Dependencies:
 - Entity API
 - cTools
 - Views


Installation
------------

Install and enable the Party module (and dependencies Entity API, Ctools,
Views) using the usual Drupal module download/enable process. Typically the
following Party sub-modules should be enabled: Party Hat, Party Profile, Party
User.

///////////////////////////////////////////////////////////////////////////////
// Overview
///////////////////////////////////////////////////////////////////////////////

Data Sets
---------

Data Sets define how data about Parties is stored. For example, a Party
installation for a library might have Data Sets created for the Profile2, User,
and LibraryCard entities.  A Data Set registers an entity type with the Party
module, by saving a set of configurations settings, which define a method
available to store data about a Party.  Is is also possible for Data Sets to
represent different configurations of the same Party plugin, for example,
different Profile2 profiles. An instance of a Data Set applied to a specific
Party is called an Attached Entity.

At the moment, the Party modules expose User Accounts, Profile2s and Customer
Profiles (Commerce Module) as Data Sets. In the future, support is planned for
Nodes, and other entity types yet to be determined.  You can also create your
own modules to allow other entities to be used as data sets (see below).

To manage your CRM’s Data Sets click “Community” on the admin toolbar and then
click “Manage Data Sets”. You will see a list of all the Data Sets that Party
Module can interact with. From here you can add new Data Sets (by clicking “Add
Profile data set”) or manage the fields and settings of existing Data Sets.


Hats
----

A Hat defines a Party type, specifying a subset of available Data Sets, if the
Hat is required, access rules and (eventually) the associated Drupal user roles.
For example, a Party with a Teacher Hat can edit the data of a Party having a
Student Hat, but not visa-versa.

A Party can have multiple Hats, and the Hats assigned to a Party can change over
time.  Hats are organized in hierarchies, with the root level of the hierarchy
normally being occupied by either the “Group” Hat or “Individual” Hat. Hats can
be created in code or through the Party UI.

A hat hierarchy for a library might look like:

Individual
--Library Patron
--Donor
--Employee
----Exempt Employee
----Non-exempt Employee
----Volunteer Employee
Group
--Publisher
----Periodicals Publication Publisher
----Books Publication Publisher
----Reference Publications Publisher
--Department
----Administration
----Circulation
----Information Technology
----Legal
----Accounting
----HR


Configuring Parties
-------------------

Click 'Community' then 'Manage Parties' to see the manage parties screen. From
here you can Manage the Fields and Display of your Party entities.


Adding a Party
--------------

Click 'Add Party' in the Navigation Menu.

Parties can also automatically be created when adding users, or when users
register for an account. For more information on user attachment, read below.


Party Labels
------------

When a party is created in the system, whether manually or programmatically, a
label is generated for it. It is possible to specify how this label is generated
by configuring any number of ‘Party Name Label’ plugins.  Under ‘Label Plugins’
in the Party configuration menu, you can sort and modify settings for these
plugins.  For example, the ‘attached entity field’ label plugin will allow you
to use a field from a data set to use for the label - thus a ‘Name’ field on an
attached profile2 entity could be used.  The sorting allows you to choose in
which order these labels are applied - thus, for parties that do not have
attached users, username will not work, but the pid example will.

To create your own Party Label plugins, refer to <Developer Documentation Section>


Drupal User Integration
-----------------------
Stand alone operation is possible using the Profile2 module. Optionally, a Drupal
user may be attached to a Party. This may be helpful when you desire your parties
to self-manage their data, be assigned roles, or be granted access to various
parts of your site.

The creation of a user or a user registration can trigger the creation of an
associated party.  You can enable this option by first ensuring that you have
Party User enabled.  Once the module is enabled, you can toggle the option
available at ‘User Integration’ in the Party configuration menu.

Lastly, you can manually attach existing users when editing a party.  (UI for
this is still being built, programatically it ‘works’)


Panels Integration
------------------

Party provides a number of plug-ins for integrating with panels. With a party
entity in the panel’s contexts it is possible to use a relationship to add any
attached entities to the context stack. There is also and attached entity
content type plug-in that will display any attached entity in a pane.

///////////////////////////////////////////////////////////////////////////////
// Developer Documentation
///////////////////////////////////////////////////////////////////////////////

Data Set API
------------

In order for a specific entity type to be used in a Data Set, it must implement
certain hooks, as defined in the Data Set API. Once you know the details of
your entity, implement hook_party_data_set_info(). For an example:

<?php
function hook_party_data_set_info() {
 $sets['data_set_name'] = array(
   'label' => 'Data Set Label',    // The readable Name of the data Set
   'entity type' => 'myentity',    // The Entity Type e.g. 'node'
   'entity bundle' => 'mybundle',  // The Entity Bundle e.g. 'article'
   'class' => 'MyEntityDataSet', // The DataSet Controller Class
   'admin' => array(                 // Paths to various admin tools
      'create' => 'url/to/add/another/entity/bundle',    // Create bundle
      'import' => 'url/to/import/another/entity/bundle', // Import bundle
      'edit' => 'url/to/edit/this/entity/bundle',        // Edit Bundle
      'manage fields' => 'url/to/manage/this/entity/bundles/fields',
      'manage fields' => 'url/to/manage/this/entity/bundles/display',
      'clone' => 'url/to/clone/this/entity/bundle',  // Clone this Bundle
      'delete' => 'url/to/delete/this/entity/bundle',// Delete this Bundle
      'export' => 'url/to/export/this/entity/bundle',// Export this bundle
    ),
 );

 return $sets;
}
?>

And implement a Data Set Controller class:

<?php
class MyEntityDataSet extends PartyDefaultDataSet {

}
?>

Your entity is now ready to be used as a data set. It will be listed on the
control panel at admin/community/datasets.


Attaching and Detaching Entities to and from Parties
----------------------------------------------------

Any entity can be attached to a Party as long as the entity type and bundle have
a Date Set definition associated with them (see above). Attaching an entity to
a party is simple:

<?php
party_attach_entity($party, $entity, $data_set_name);
?>

Where:

$party is the Party object.
$entity is the Entity object (or stdClass)
$data_set_name is the name of the data set that has the corresponding entity type
and entity bundle keys.

Likewise, to detach an entity from a party call:
<?php
party_detach_entity($party, $entity, $data_set_name);
?>


Custom Data Set Forms
---------------------

The Data Set API has a Form Embedding system that allows any attached entity form
to be embedded into the various party forms around the system. The default form
callback work for simple, fieldable entities but for anything more complicated
custom callbacks will be required.

To override the default dataset form callbacks and the 'form callback' key to
the data set array in hook_party_data_set_info like so:

<?php
/**
* Implements hook_party_data_set_info()
*/
function party_user_party_data_set_info() {
 $sets['user'] = array(
   'label' => t("User account"),
   :
   :
   'form callback' => "party_user_form_user",
 );
 return $sets;
}
?>

This overrides party_default_attached_entity_form with party_user_form_user and
optionally allows you to override the _validate and _submit functions. The
first form callback returns a portion of the final form array to be put in
$form[$attached_entity->hash()]. All form callbacks take the following four
arguments:

$form – the full form array
$form_state – the form state
$attached_entity – the attached entity wrapper. The entity itself is in
                   $attached_entity->entity.
$party – the party object

For a more detailed example of overriding the default form callbacks see the
Party User module.
