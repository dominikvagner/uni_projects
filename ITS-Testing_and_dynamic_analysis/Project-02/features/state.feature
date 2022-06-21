#     ___ _   __
#    / _ \ | / /
#   / // / |/ /
#  /____/|___/
#
# @author Dominik Vagner, xvagne10
# @date 2022-04-17

Feature: Different levels of content published state.

    Content types can have different levels of published state. 
    This state dictates which types of users can see the content.
    Producents can change the published state of the content.
    # As an example, we will show the content of the "Use Case" type.
    # For other types tests would be fairly similiar. So to keep
    # the number of artifacts and tests to a reasonable amount.

    Scenario: Consumer can only see published content
        Given I am not logged in.
        When I go to the Use Cases page
        Then Only published content is displayed

    Scenario: Producent can see published and private content
        Given I am logged in as Producent
        When I go to the Use Cases page
        Then Both published and private content is displayed

    Scenario: Change product published state
        Given I am logged in as Producent
            And I am on the detail page of a Use Case
        When I click on the first item in the state option menu
        Then Confirmation of the state change appears
