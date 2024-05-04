DROP DATABASE IF EXISTS shopdb;
CREATE DATABASE shopdb;
USE shopdb;

CREATE TABLE shops (
    shop_id INT PRIMARY KEY AUTO_INCREMENT,
    shop_name VARCHAR(255) UNIQUE NOT NULL,
    shop_location VARCHAR(255) NOT NULL
);

CREATE TABLE categories (
    category_id INT PRIMARY KEY AUTO_INCREMENT,
    category_name VARCHAR(255) NOT NULL
);

CREATE TABLE products (
    product_id INT PRIMARY KEY AUTO_INCREMENT,
    product_name VARCHAR(255) NOT NULL,
    category_id INT,
    price DECIMAL(10, 2) NOT NULL,
    product_description TEXT
);

CREATE TABLE stock (
    stock_id INT PRIMARY KEY AUTO_INCREMENT,
    product_id INT,
    shop_id INT,
    stock_quantity INT NOT NULL,
    last_stocked DATE
);

CREATE TABLE orders (
    order_id INT PRIMARY KEY AUTO_INCREMENT,
    product_id INT,
    shop_id INT,
    order_quantity INT NOT NULL,
    order_date DATE
);