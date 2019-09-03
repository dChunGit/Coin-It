package com.simplex.coin2credit.database

import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.Query

@Dao
interface TransactionDao {
    @get:Query("SELECT * FROM transactionDB")
    val all: List<Transaction>

    @Query("SELECT * FROM transactionDB where date LIKE :date")
    fun findByDate(date: String): Transaction

    @Query("SELECT COUNT(*) from transactionDB")
    fun countTransactions(): Int

    @Insert
    fun insertAll(vararg transactions: Transaction)

    @Delete
    fun delete(transaction: Transaction)
}
