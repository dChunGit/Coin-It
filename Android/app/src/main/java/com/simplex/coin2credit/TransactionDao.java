package com.simplex.coin2credit;

import android.arch.persistence.room.Dao;
import android.arch.persistence.room.Delete;
import android.arch.persistence.room.Insert;
import android.arch.persistence.room.Query;

import java.util.List;

@Dao
public interface TransactionDao {
    @Query("SELECT * FROM transactionDB")
    List<Transaction> getAll();

    @Query("SELECT * FROM transactionDB where date LIKE :date")
    Transaction findByDate(String date);

    @Query("SELECT COUNT(*) from transactionDB")
    int countTransactions();

    @Insert
    void insertAll(Transaction... transactions);

    @Delete
    void delete(Transaction transaction);
}
