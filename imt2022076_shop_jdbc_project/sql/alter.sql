ALTER TABLE products
    ADD CONSTRAINT fk_cat FOREIGN KEY (category_id) REFERENCES categories(category_id) ON DELETE CASCADE;

ALTER TABLE stock
    ADD CONSTRAINT fk_pid FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
    ADD CONSTRAINT fk_sid FOREIGN KEY (shop_id) REFERENCES shops(shop_id) ON DELETE CASCADE;

ALTER TABLE orders
    ADD CONSTRAINT fk_opid FOREIGN KEY (product_id) REFERENCES products(product_id) ON DELETE CASCADE,
    ADD CONSTRAINT fk_osid FOREIGN KEY (shop_id) REFERENCES shops(shop_id) ON DELETE CASCADE;