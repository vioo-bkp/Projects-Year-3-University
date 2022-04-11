"""
This module represents the Marketplace.

Computer Systems Architecture Course
Assignment 1
March 2021
"""


class Marketplace:
    """
    Class that represents the Marketplace. It's the central part of the implementation.
    The producers and consumers use its methods concurrently.
    """
    def __init__(self, queue_size_per_producer):
        """
        Constructor

        :type queue_size_per_producer: Int
        :param queue_size_per_producer: the maximum size of a queue associated with each producer
        """
        # define variables for the marketplace
        self.queue_size_per_producer = queue_size_per_producer
        self.producer_id = 0
        self.consumer_id = 0
        self.cart_id = 0
        self.producer = {}
        self.consumer = {}
        self.unique_cart = {}

    def register_producer(self):
        """
        Returns an id for the producer that calls this.
        """
        # Initialize every producer with an empty list
        # And return an id for the producer that will be used to publish
        self.producer_id = self.producer_id + 1
        self.producer[self.producer_id] = []
        return self.producer_id

    def publish(self, producer_id, product):
        """
        Adds the product provided by the producer to the marketplace

        :type producer_id: String
        :param producer_id: producer id

        :type product: Product
        :param product: the Product that will be published in the Marketplace

        :returns True or False. If the caller receives False, it should wait and then try again.
        """
        # Verify if queue_size_per_producer is not exceeded
        # If not, add the product to the producer's list
        # If yes, return False
        if len(self.producer[producer_id]) < self.queue_size_per_producer:
            self.producer[producer_id].append(product)
            return True
        return False

    def new_cart(self):
        """
        Creates a new cart for the consumer

        :returns an int representing the cart_id
        """
        # Initialize every cart with an empty list
        # And return an id for the cart that will be used to add and remove products
        self.consumer_id = self.consumer_id + 1
        self.consumer[self.consumer_id] = []
        return self.consumer_id

    def add_to_cart(self, cart_id, product):
        """
        Adds a product to the given cart. The method returns

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to add to cart

        :returns True or False. If the caller receives False, it should wait and then try again
        """
        # For every producer, verify if the product is in the producer's list
        # If yes, remove the product from the producer's list and add it to the cart
        # If not, return False
        # Loop using tuple unpacking to check if the product is in the producer's list
        for producer_id, products in self.producer.items():
            if product in products:
                self.producer[producer_id].remove(product)
                self.consumer[cart_id].append(product)
                self.unique_cart[cart_id] = producer_id
                return True
        return False

    def remove_from_cart(self, cart_id, product):
        """
        Removes a product from cart.

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to remove from cart
        """
        # For every product in the cart, verify if the product is in the cart's list
        # If yes, remove the product from the cart's list and add it to the producer's list
        if product in self.consumer[cart_id]:
            product_type = self.unique_cart[cart_id]
            consumer_cart = self.consumer[cart_id]
            consumer_cart.remove(product)
            self.producer[product_type].append(product)

    def place_order(self, cart_id):
        """
        Return a list with all the products in the cart.

        :type cart_id: Int
        :param cart_id: id cart
        """
        # We use map to get the list of products in the cart
        # Lambda function to get the list of products in the cart
        return map(lambda x: x, self.consumer[cart_id])
