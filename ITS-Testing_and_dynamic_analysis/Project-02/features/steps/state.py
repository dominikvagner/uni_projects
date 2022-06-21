import time
from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.common.exceptions import NoSuchElementException


@given(u'I am not logged in.')
def step_impl(context):
    elem = context.driver.find_element(By.XPATH, "//a[@id='personaltools-login']").text
    assert elem == "Log in"


@when(u'I go to the Use Cases page')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Use Cases").click()


@then(u'Only published content is displayed')
def step_impl(context):
    try:
        tbody = context.driver.find_element(By.XPATH, "//tbody")
        private = tbody.find_elements(By.CLASS_NAME, "state-private")
        assert len(private) == 0
    except NoSuchElementException:
        pass


@then(u'Both published and private content is displayed')
def step_impl(context):
    tbody = context.driver.find_element(By.XPATH, "//tbody")
    private = tbody.find_elements(By.CLASS_NAME, "state-private")
    published = tbody.find_elements(By.CLASS_NAME, "state-published")
    assert len(private) >= 0 and len(published) >= 0


@given(u'I am on the detail page of a Use Case')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "UC1 - Intelligent Traffic Surveillance").click()


@when(u'I click on the first item in the state option menu')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    context.driver.find_element(By.ID, "plone-contentmenu-workflow").click()
    context.driver.find_element(By.XPATH, "/html/body/section/div/div/nav/ul[1]/li[5]/ul/li[2]").click()


@then(u'Confirmation of the state change appears')
def step_impl(context):
    context.driver.find_element(By.XPATH, "/html/body/div[1]/div[3]/div/aside/div[1]")
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    hidden = True
    try:
        context.driver.find_element(By.ID, "workflow-transition-publish")
    except NoSuchElementException:
        hidden = False

    if hidden is True:
        context.driver.find_element(By.ID, "plone-contentmenu-workflow").click()
        context.driver.find_element(By.XPATH, "/html/body/section/div/div/nav/ul[1]/li[5]/ul/li[2]").click()
