INSERT INTO shops (shop_name, shop_location) VALUES
('shop_1', 'location_1'),
('shop_2', 'location_2'),
('shop_3', 'location_3');

INSERT INTO categories (category_name) VALUES
('category_1'),
('category_2'),
('category_3');

INSERT INTO products (product_name, category_id, price, product_description) VALUES
('product_1', 1, 10.00, 'desc_1'),
('product_2', 2, 20.00, 'desc_2'),
('product_3', 3, 30.00, 'desc_3'),
('product_4', 1, 40.00, 'desc_4'),
('product_5', 2, 50.00, 'desc_5');

INSERT INTO stock (product_id, shop_id, stock_quantity, last_stocked) VALUES
(1, 1, 100, '2024-04-11'),
(1, 2, 150, '2024-03-12'),
(1, 3, 150, '2024-04-13'),
(2, 1, 180, '2024-03-14'),
(2, 2, 120, '2024-04-15'),
(2, 3, 120, '2024-03-16'),
(3, 1, 200, '2024-04-17'),
(3, 2, 200, '2024-03-18'),
(3, 3, 200, '2024-04-19'),
(4, 1, 120, '2024-03-20'),
(4, 2, 100, '2024-04-21'),
(4, 3, 100, '2024-04-22'),
(5, 1, 180, '2024-04-23'),
(5, 2, 120, '2024-04-24'),
(5, 3, 120, '2024-04-25');

INSERT INTO orders(product_id, shop_id, order_quantity, order_date) VALUES
(1, 1, 10, '2024-04-01'),
(2, 2, 20, '2024-03-11'),
(3, 3, 10, '2024-04-02'),
(4, 1, 10, '2024-03-12'),
(5, 2, 10, '2024-03-04'),
(1, 3, 20, '2024-04-09'),
(2, 1, 50, '2024-03-03'),
(3, 2, 20, '2024-03-09'),
(4, 3, 10, '2024-04-11'),
(5, 3, 20, '2024-04-12');