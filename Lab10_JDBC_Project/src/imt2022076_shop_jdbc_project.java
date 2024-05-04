import java.sql.*;
import java.util.*;

public class imt2022076_shop_jdbc_project {
    static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
    static final String DB_URL = "jdbc:mysql://localhost:3306/shopdb";
    static final String USER = "root";
    static final String PASSWORD = "admin";

    public static void main (String[] args){   
        Connection conn = null;
        Statement stmt = null;
        Scanner sc = new Scanner(System.in);
        ResultSet rs=null;
        try{
            Class.forName(JDBC_DRIVER);
            conn = DriverManager.getConnection(DB_URL, USER, PASSWORD);
            conn.setAutoCommit(false);
            stmt = conn.createStatement();
            
            System.out.println("""
                    ************************************************************
                    1. Add a shop
                    2. Add a category
                    3. Add a product
                    4. Add an order
                    5. Update a product
                    6. Update shop location
                    7. Modify stock of a product
                    8. Remove a category
                    9. Remove a product
                    10. See all shops
                    11. See all categories
                    12. See all products
                    13. See all products in a specific shop
                    14. See all orders for a specific shop
                    15. See all shops with stock between 100 and 200 for a product
                    16. See the most ordered product
                    17. See the most active shop
                    18. See total orders done in the last month
                    19. See the average order price
                    20. Exit
                    ************************************************************
                    """
            );
            while (true) {
                System.out.print("Enter your choice: ");
                int choice = sc.nextInt();
                switch (choice) {
                    case 1: // Add a shop
                        System.out.print("Enter shop name: ");
                        sc.nextLine();
                        String shopName = sc.nextLine();
                        System.out.print("Enter shop location: ");
                        String shopLocation = sc.nextLine();
                        String query = "INSERT INTO shops (shop_name, shop_location) VALUES ('" + shopName + "', '" + shopLocation + "');";
                        int output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Shop added successfully\n");
                        else throw new SQLException("Failed to add shop");
                        conn.commit();
                        break;
                    case 2: // Add a category
                        System.out.print("Enter category name: ");
                        sc.nextLine();
                        String categoryName = sc.nextLine();
                        query = "INSERT INTO categories (category_name) VALUES ('" + categoryName + "');";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Category added successfully\n");
                        else throw new SQLException("Failed to add category");
                        conn.commit();
                        break;
                    case 3: // Add a product
                        System.out.print("Enter product name: ");
                        sc.nextLine();
                        String productName = sc.nextLine();
                        System.out.print("Enter category ID: ");
                        int categoryId = sc.nextInt();
                        System.out.print("Enter price: ");
                        double price = sc.nextDouble();
                        System.out.print("Enter product description: ");
                        sc.nextLine();
                        String productDescription = sc.nextLine();
                        query = "INSERT INTO products (product_name, category_id, price, product_description) " +
                                "VALUES ('" + productName + "', " + categoryId + ", " + price + ", '" + productDescription + "');";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Product added successfully\n");
                        else throw new SQLException("Failed to add product");
                        conn.commit();
                        break;
                    case 4: // Add an order
                        System.out.print("Enter product ID: ");
                        int productId = sc.nextInt();
                        System.out.print("Enter shop ID: ");
                        int shopId = sc.nextInt();
                        System.out.print("Enter order quantity: ");
                        int orderQuantity = sc.nextInt();
                        String checkStockQuery = "SELECT stock_quantity FROM stock WHERE product_id = " + productId + " AND shop_id = " + shopId + ";";
                        int availableStock = 0;
                        String addOrderQuery = "INSERT INTO orders (product_id, shop_id, order_quantity, order_date) " +
                        "VALUES (" + productId + ", " + shopId + ", " + orderQuantity + ", CURDATE());";
                        output = stmt.executeUpdate(addOrderQuery);
                        rs = stmt.executeQuery(checkStockQuery);
                        if (rs.next()){
                            availableStock = rs.getInt("stock_quantity");
                            if (orderQuantity <= availableStock){
                                if (output == 1) {
                                    String reduceStockQuery = "UPDATE stock SET stock_quantity = stock_quantity - " + orderQuantity +
                                    " WHERE product_id = " + productId + " AND shop_id = " + shopId + ";";
                                    output = stmt.executeUpdate(reduceStockQuery);
                                    if (output == 1) System.out.println("Order added successfully\n");
                                    
                                }
                            }
                            else throw new SQLException("Insufficient Stock");
                        }
                        conn.commit();
                        break;
                    case 5: // Update a product
                        System.out.print("Enter product ID: ");
                        productId = sc.nextInt();
                        System.out.print("Enter new price: ");
                        price = sc.nextDouble();
                        System.out.print("Enter new product description: ");
                        sc.nextLine();
                        productDescription = sc.nextLine();
                        query = "UPDATE products SET price = "+price+", product_description = '" + productDescription + "' WHERE product_id = " + productId + ";";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Product updated successfully\n");
                        else throw new SQLException("Error in updating product");
                        conn.commit();
                        break;
                    case 6: // Update shop location
                        System.out.print("Enter shop ID: ");
                        shopId = sc.nextInt();
                        System.out.print("Enter new location: ");
                        sc.nextLine();
                        shopLocation = sc.nextLine();
                        query = "UPDATE shops SET shop_location = '" + shopLocation + "' WHERE shop_id = " + shopId + ";";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Shop location updated successfully\n");
                        else throw new SQLException("Error in updating shop details");
                        conn.commit();
                        break;
                    case 7: // Restock a product
                        System.out.print("Enter product ID: ");
                        productId = sc.nextInt();
                        System.out.print("Enter shop ID: ");
                        shopId = sc.nextInt();
                        System.out.print("Enter quantity of stock to add: ");
                        int restockQuantity = sc.nextInt();
                        checkStockQuery = "SELECT * FROM stock WHERE product_id = " + productId + " AND shop_id = " + shopId + ";";
                        rs = stmt.executeQuery(checkStockQuery);                        
                        if (rs.next()){
                            String updateStockQuery = "UPDATE stock SET stock_quantity = stock_quantity + " + restockQuantity +
                            ", last_stocked = CURDATE() WHERE product_id = " + productId + " AND shop_id = " + shopId + ";";
                            output = stmt.executeUpdate(updateStockQuery);
                            if (output == 1) System.out.println("Stock added successfully\n");
                            else throw new SQLException("Error in restocking product");
                        }
                        else {
                            String insertStockQuery = "INSERT INTO stock (product_id, shop_id, stock_quantity, last_stocked) " +
                            "VALUES (" + productId + ", " + shopId + ", " + restockQuantity + ", CURDATE());";
                            output = stmt.executeUpdate(insertStockQuery);
                            if (output == 1) System.out.println("Product stocked successfully\n");
                            else throw new SQLException("Error in stocking product");
                        }
                        conn.commit();
                        break;
                    case 8: // Remove a category
                        System.out.print("Enter category ID: ");
                        categoryId = sc.nextInt();
                        query = "DELETE FROM categories WHERE category_id = " + categoryId + ";";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Category removed successfully\n");
                        else throw new SQLException("Error in removing category");
                        conn.commit();
                        break;
                    case 9: // Remove a product
                        System.out.print("Enter product ID: ");
                        productId = sc.nextInt();
                        query = "DELETE FROM products WHERE product_id = " + productId + ";";
                        output = stmt.executeUpdate(query);
                        if (output == 1) System.out.println("Product removed successfully\n");
                        else throw new SQLException("Error in removing product");
                        conn.commit();
                        break;
                    case 10: // See all shops
                        query = "SELECT * FROM shops;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Shop ID: " + rs.getInt("shop_id"));
                            System.out.println("Shop Name: " + rs.getString("shop_name"));
                            System.out.println("Shop Location: " + rs.getString("shop_location"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 11: // See all categories
                        query = "SELECT * FROM categories;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Category ID: " + rs.getInt("category_id"));
                            System.out.println("Category Name: " + rs.getString("category_name"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 12: // See all products
                        query = "SELECT * FROM products;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Product ID: " + rs.getInt("product_id"));
                            System.out.println("Product Name: " + rs.getString("product_name"));
                            System.out.println("Category ID: " + rs.getInt("category_id"));
                            System.out.println("Price: " + rs.getDouble("price"));
                            System.out.println("Description: " + rs.getString("product_description"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 13: // See all products in a specific shop
                        System.out.print("Enter shop ID: ");
                        shopId = sc.nextInt();
                        query = "SELECT p.product_id, p.product_name, s.stock_quantity " +
                                "FROM products p INNER JOIN stock s ON p.product_id = s.product_id " +
                                "WHERE s.shop_id = "+shopId+";";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Product ID: " + rs.getInt("product_id"));
                            System.out.println("Product Name: " + rs.getString("product_name"));
                            System.out.println("Stock Quantity: " + rs.getInt("stock_quantity"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 14: // See all orders for a specific shop
                        System.out.print("Enter shop ID: ");
                        shopId = sc.nextInt();
                        query = "SELECT * FROM orders WHERE shop_id = "+shopId+";";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Order ID: " + rs.getInt("order_id"));
                            System.out.println("Product ID: " + rs.getInt("product_id"));
                            System.out.println("Order Quantity: " + rs.getInt("order_quantity"));
                            System.out.println("Order Date: " + rs.getString("order_date"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 15: // See all shops with stock between 100 and 200 for a product
                        System.out.print("Enter product ID: ");
                        productId = sc.nextInt();
                        query = "SELECT s.shop_id, s.shop_name, st.stock_quantity AS stock_quantity " +
                                "FROM shops s " +
                                "INNER JOIN stock st ON s.shop_id = st.shop_id AND st.product_id = "+productId+" " +
                                "WHERE st.stock_quantity > 100 AND st.stock_quantity < 200;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        while (rs.next()) {
                            System.out.println("Shop ID: " + rs.getInt("shop_id"));
                            System.out.println("Shop Name: " + rs.getString("shop_name"));
                            System.out.println("Stock Quantity: " + rs.getInt("stock_quantity"));
                            System.out.println();
                        }
                        System.out.println("********************");
                        break;
                    case 16: // See the most ordered product
                        query = "SELECT product_id, COUNT(*) AS total_orders FROM orders GROUP BY product_id ORDER BY total_orders DESC LIMIT 1;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        if (rs.next()) {
                            System.out.println("Most ordered product ID: " + rs.getInt("product_id"));
                            System.out.println("Total orders: " + rs.getInt("total_orders"));
                        }
                        System.out.println("********************");
                        break;
                    case 17: // See the most active shop
                        query = "SELECT s.shop_id, COUNT(*) AS total_orders FROM orders o INNER JOIN shops s ON o.shop_id = s.shop_id GROUP BY s.shop_id ORDER BY total_orders DESC LIMIT 1;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        if (rs.next()) {
                            System.out.println("Most active shop ID: " + rs.getInt("shop_id"));
                            System.out.println("Total orders: " + rs.getInt("total_orders"));
                        }
                        System.out.println("********************");
                        break;
                    case 18: // See total orders done in the last month
                        query = "SELECT COUNT(*) AS total_orders FROM orders WHERE order_date >= DATE_SUB(CURDATE(), INTERVAL 1 MONTH);";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        if (rs.next()) {
                            System.out.println("Total orders in the last month: " + rs.getInt("total_orders"));
                        }
                        System.out.println("********************");
                        break;
                    case 19: // See the average order price
                        query = "SELECT AVG(order_quantity * price) AS avg_order_price FROM orders o " +
                                "INNER JOIN products p ON o.product_id = p.product_id;";
                        rs = stmt.executeQuery(query);
                        System.out.println("********************");
                        if (rs.next()) {
                            System.out.println("Average order price: " + rs.getFloat("avg_order_price"));
                        }
                        System.out.println("********************");
                        break;
                    case 20: // Exit
                        break;
                    default:
                        System.out.println("Not a valid choice\n");
                        break;
                }
                if (choice == 20) break;
            }
            sc.close();
            stmt.close();
            conn.close();
            rs.close();
        }
        catch (SQLException se) {
            se.printStackTrace();
            // If there is an error then rollback the changes.
            System.out.println("Rolling back data here....");
            try { if(conn!=null) conn.rollback(); }
            catch(SQLException se2){
                System.out.println("Rollback failed....");
                    se2.printStackTrace();
            }
        }
        catch (Exception e){
            e.printStackTrace();
        }
        finally{
            try { if (stmt != null) stmt.close(); }
            catch (SQLException se2) {} // nothing we can do
            try { if (conn != null) conn.close();}
            catch (SQLException se) {se.printStackTrace();}
        }
        System.out.println("\nEnd of Code\n");
    }
}