"""
This module represents the Consumer.

Computer Systems Architecture Course
Assignment 1
March 2021
"""

from threading import Thread
from time import sleep


class Consumer(Thread):
    """
    Class that represents a consumer.
    """

    def __init__(self, carts, marketplace, retry_wait_time, **kwargs):
        """
        Constructor.

        :type carts: List
        :param carts: a list of add and remove operations

        :type marketplace: Marketplace
        :param marketplace: a reference to the marketplace

        :type retry_wait_time: Time
        :param retry_wait_time: the number of seconds that a producer must wait
        until the Marketplace becomes available

        :type kwargs:
        :param kwargs: other arguments that are passed to the Thread's __init__()
        """
        Thread.__init__(self)
        self.name = kwargs["name"]
        self.carts = carts
        self.marketplace = marketplace
        self.retry_wait_time = retry_wait_time

    def run(self):
        for unique_cart in self.carts:
            # Create a unique unique_cart id for every consumer
            uniq_cart = self.marketplace.new_cart()

            for operation in unique_cart:
                # Remove the product from the producer's list
                if operation["type"] == "remove":
                    while int(operation["quantity"]) > 0:
                        # Remove an operation from the unique_cart.
                        self.marketplace.remove_from_cart(uniq_cart, operation["product"])
                        operation["quantity"] -= 1
                # Add the product to the unique_cart.
                elif operation["type"] == "add":
                    while int(operation["quantity"]) > 0:
                        wish_list = self.marketplace.add_to_cart(uniq_cart, operation["product"])
                        # If the product is not available, wait until it is.
                        if not wish_list:
                            sleep(self.retry_wait_time)
                            continue
                        operation["quantity"] -= 1
            # Checkout the unique_cart.
            product_acquired = self.marketplace.place_order(uniq_cart)
            # Display products:
            for product in product_acquired:
                print(str(self.name) + " " + "bought" + " " + str(product))
