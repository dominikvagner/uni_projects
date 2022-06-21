#!/usr/bin/env python3
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.common.exceptions import NoSuchElementException


def before_all(context):
    context.driver = webdriver.Remote(command_executor='http://localhost:4444/wd/hub',
                                      desired_capabilities=DesiredCapabilities.FIREFOX)
    context.driver.implicitly_wait(15)
    context.driver.get("http://localhost:8080/repo")
    context.driver.maximize_window()


def after_scenario(context, scenario):
    context.driver.get("http://localhost:8080/repo")
    try:
        context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
        context.driver.find_element(By.CSS_SELECTOR, "#portal-personaltools span:nth-child(2)").click()
        context.driver.find_element(By.ID, "personaltools-logout").click()
    except NoSuchElementException:
        pass


def after_all(context):
    context.driver.quit()
